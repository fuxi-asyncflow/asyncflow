#include "spdlog/sinks/base_sink.h"
#include <Python.h>
#include <object.h>
#include <array>

namespace spdlog
{
	class python_logger_sink : public sinks::sink
	{
	public:
		explicit python_logger_sink(PyObject* pylogger)
			: py_logger_(pylogger)
		{
			std::array<const char*, 7> level_names = { "", "debug", "info", "warning", "error", "fatal", "" };
			for (int i = 0; i < 7; i++)
			{
				func_names_[i] = nullptr;
			}

			for(int i=1; i<6; i++)
			{
				if (!PyObject_HasAttrString(pylogger, level_names[i]))
					continue;
				auto* func = PyObject_GetAttrString(pylogger, level_names[i]);
				if(func != nullptr && PyCallable_Check(func))
				{
					Py_XINCREF(func);
					func_names_[i] = func;
				}
			}
			Py_XINCREF(py_logger_);
		}

		virtual ~python_logger_sink()
		{
			int initialized = Py_IsInitialized();
			if (initialized)
			{
				Py_XDECREF(py_logger_);
				for (int i = 0; i < func_names_.size(); i++)
				{
					Py_XDECREF(func_names_[i]);
				}
			}
		}		

	public:
		void log(const details::log_msg &msg) override
		{
			fmt::memory_buffer formatted;
			formatter_->format(msg, formatted);
			auto* str = PyUnicode_FromStringAndSize(formatted.data(), formatted.size());
			auto* func = func_names_[static_cast<size_t>(msg.level)];
			if (func != nullptr)
				PyObject_CallFunctionObjArgs(func, str, nullptr);
			Py_XDECREF(str);
			return;
		}
		
		void set(PyObject* newpy_logger)
		{
			if (py_logger_ != newpy_logger)
			{
				Py_XDECREF(py_logger_);
				py_logger_ = newpy_logger;
				Py_XINCREF(py_logger_);
			}
		}

		void flush() override {}
		void set_pattern(const std::string &pattern) override
		{
			formatter_ = std::unique_ptr<spdlog::formatter>(new pattern_formatter(pattern));
		}
		void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override
		{
			formatter_ = std::move(sink_formatter);
		}

	private:
		PyObject* py_logger_;
		std::array<PyObject*, 7> func_names_;	
	};
}

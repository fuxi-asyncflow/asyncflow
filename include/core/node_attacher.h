#pragma once
#include <ctime>
#include <vector>
#include <map>
#include "ratas.h"

namespace asyncflow
{
	namespace core
	{
		class Node;
		class Chart;
		class INodeAttacher
		{			
		public:
			enum Type
			{
				TIMER = 1,
				SUBCHART = 2,
				WAITALL = 3
			};
			INodeAttacher(Node* node, Type type)
				: node_(node)
				, type_(type)
			{}
			virtual ~INodeAttacher(){}
			virtual void Stop() = 0;
			Type GetType() const { return type_; }
		protected:
			Node* node_;
			Type type_;
		};

		//timer for wait node
		class NodeTimer : public INodeAttacher, public TimerEventInterface
		{
		public:
			NodeTimer(Node* node, int milliseconds);
			~NodeTimer() override;
			void execute() override;
			void Stop() override;
			uint64_t GetTimeInterval() { return time_interval_; }
			void SetTimeInterval(int tm) { time_interval_ = tm; }
			void Register(Node* node, uint64_t millisecond);
            
			uint64_t time_interval_;
			uint64_t left_time_;
	
		};

		class Subchart : public INodeAttacher
		{
		public:
			Subchart(Node* node);
			~Subchart();
			void Stop() override;
			void SetChart(Chart* chart) { chart_ = chart; }
			Chart* GetChart() { return chart_; }			
		private:
			Chart* chart_;
		};

		class NodeWaitAll : public INodeAttacher
		{
		public:
			NodeWaitAll(Node* node);
			~NodeWaitAll();

			void Init(const std::vector<int>& args);
			bool Run(int id);
			void Stop() override;
		private:
			std::map<int, bool> node_ids_;
		};
	}
}
#define PY_SSIZE_T_CLEAN
#include <Python.h>
#include <cstring>
#include <vector>
#include <string>

void run_testfile(const char* file_name)
{
    int len = strlen(file_name);
    FILE* fp = fopen(file_name, "rb");
    if (fp == nullptr)
        return;
    PyRun_SimpleFile(fp, file_name);
}

const char* get_files =

"def get_files(path):\n"
"    import os\n"
"    files = os.listdir(path)\n"
"    py_files = []\n"
"    for file in files:\n"
"        if file.startswith('test'):\n"
"            py_files.append(file)\n"
"            #print(file)\n"
"    return py_files"
"";

int main(int argc, char* argv[])
{
#ifdef USING_PYTHON2
    auto* program = argv[0];
#else
    wchar_t* program = Py_DecodeLocale(argv[0], NULL);
#endif
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);  /* optional but recommended */
    Py_Initialize();
    //PyRun_SimpleString("from time import time,ctime\n"
    //    "print('Today is', ctime(time()))\n"); 
    auto* pGlobal = PyDict_New();
    auto* pModule = PyModule_New("mymod");
    PyModule_AddStringConstant(pModule, "__file__", "");
    PyModule_AddObject(pModule, "os", PyImport_AddModule("os"));
    auto* pLocal = PyModule_GetDict(pModule);

    PyObject* get_files_func = PyRun_String(get_files, Py_file_input, pGlobal, pLocal);
    printf("get_files_func %p\n", get_files_func);
    auto* pFunc = PyObject_GetAttrString(pModule, "get_files");

    auto* test_folder = argv[1];

    auto* py_args = PyTuple_New(1);
    PyTuple_SetItem(py_args, 0, PyUnicode_FromString(test_folder));
    auto files_object = PyObject_CallObject(pFunc, py_args);
    Py_DECREF(py_args);
    
    int size = PyList_GET_SIZE(files_object);
    std::vector<std::string> files (size);
    for(int i=0; i<size; i++)
    {
#ifdef USING_PYTHON2
        files[i] = std::string(test_folder) + "/" + PyString_AsString(PyList_GetItem(files_object, i));
#else
        files[i] = std::string(test_folder) + "/" + PyUnicode_AsUTF8(PyList_GetItem(files_object, i));
#endif
        printf("%s\n", files[i].c_str());
        run_testfile(files[i].c_str());
    }

#ifdef USING_PYTHON2
    Py_Finalize();
#else
    if (Py_FinalizeEx() < 0) {
        exit(120);
    }
    PyMem_RawFree(program);
#endif
    return 0;
}
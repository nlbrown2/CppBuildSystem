#include <regex>
#include <iostream>
#include "test.h"
#include <unistd.h> //for close
#include <vector>
#include <iterator>
#include <set>
#include <map>
#include <algorithm>
#include <stdexcept>
#include <fstream>
#include <unistd.h>
  #include "test.h"
#include "test2.h"
#include "nested.h" //this includes an inner file.
#include <fcntl.h> //for open
#include <sys/mman.h> //for mmap
#include <sys/stat.h> //for fstat
#include <thread> //to determine number of cores
#include "File.h"
#include "FileFactory.h"
#include "Buildfile.h"
using namespace std;
using Stat = struct stat;

bool operator <(const timespec& lhs, const timespec& rhs)
{
    if (lhs.tv_sec == rhs.tv_sec)
        return lhs.tv_nsec < rhs.tv_nsec;
    else
        return lhs.tv_sec < rhs.tv_sec;
}

bool should_recompile(const string& cpp_filename, const string& object_filename) {
    try {
        const ObjectFile& target = FileFactory::get().get_obj_file(object_filename);
        const CppFile& maincpp = FileFactory::get().get_cpp_file(cpp_filename);
        bool should_recompile = false;
        if(target.get_statistics().st_mtimespec < maincpp.get_statistics().st_mtimespec) {
            should_recompile = true;
        }
        else {
            const set<string>& headers = maincpp.get_headers();
            for(auto& name : headers) {
                const CppFile& header = FileFactory::get().get_cpp_file(name);
                if(target.get_statistics().st_mtimespec < header.get_statistics().st_mtimespec) {
                    should_recompile = true;
                    break;
                }
            }
        }
        return should_recompile;
    } catch(runtime_error& e) {
        return true;
    }
}

pid_t run_process(const vector<string>& args) {
    //turn std::string args to c string args
    pid_t pid = fork();
    if(pid) {
        //print the command of the child process
        copy(args.begin(), args.end(), ostream_iterator<const string&>(cout, " "));
        cout << endl;
    } else {
        vector<char*> c_args{args.size()};
        transform(args.begin(), args.end(), c_args.begin(), [](const string& s){return const_cast<char*>(s.c_str());});
        c_args.push_back(nullptr); //null terminate the array
        if(execvp(c_args[0], c_args.data()) == -1) {
            cerr << "error opening subprocess: " << endl;
            switch(errno) {
                case ENOENT:
                    cerr << "does not exit" << endl;
                    break;
                case ENOEXEC:
                    cerr << "cannot execute" << endl;
                    break;
                case EFAULT:
                    cerr << "outside address space" << endl;
                    break;
                case EACCES:
                    cerr << "execute permission denied" << endl;
                    break;
                default:
                    cerr << "some other error" << endl;
                    break;
            }
        }
        exit(0);
    }
    return pid;
}

int main(int argc, char** argv) {
    //TODO: check if buildfile is more recent then executable. If so, recompile everything and relink everything
    string_view profile = "default";
    bool run_postcmd = true;
    if(--argc) {
        if(!strcmp(*++argv, "--nopostcmd") || !strcmp(*argv, "-n")) {
          run_postcmd = false;
          ++argv;
        }
        if(*argv)
          //user has specified a profile to be used
          profile = *argv; //do ++ to skip past executable name
    }
    Buildfile buildfile{profile};
    const vector<string>& cpp = buildfile.get_cpp_filenames();
    const vector<string>& objects = buildfile.get_object_filenames();
    int cores = thread::hardware_concurrency();
    cores = min(cores, static_cast<int>(cpp.size()));
    cout << "using up to: " << cores << " processes at once" << endl;
    set<pid_t> process_ids;
    bool should_relink = false;
    for(size_t i = 0; i < objects.size(); ++i) {
        if(should_recompile(cpp[i], objects[i])) {
            if(process_ids.size() == static_cast<size_t>(cores)) {
                //if we are using the max amount of processes, wait for a child
                //this should allow any child to finish at any time and just
                //use that instead
                int status = 0;
                pid_t wpid = wait(&status);
                if(WEXITSTATUS(status)) {
                    cerr << "status was: " << status << " aborting linking!" << endl;
                    return 1;
                }
                process_ids.erase(wpid);
            }
            process_ids.insert(run_process(buildfile.get_compile_command(cpp[i], objects[i])));
            should_relink = true;
        }
    }
    //This checks if the executable exists. If not, we have to relink
    Stat exe_stat;
    if(stat(buildfile.get_execuatable_name().c_str(), &exe_stat) == -1 && errno == ENOENT)
        should_relink = true;
    if(should_relink) {
        //wait for all children to finish compiling the object files
        pid_t wpid;
        int status = 0;
        while ((wpid = wait(&status)) > 0) {
            if(WEXITSTATUS(status)) {
                cerr << "status was: " << status << " aborting linking!" << endl;
                return 1;
            }
        }
        cout << buildfile.get_link_command(objects) << endl;
        system(buildfile.get_link_command(objects).c_str());
        if(!buildfile.get_postlink_command().empty() && run_postcmd) {
            cout << "running: " << buildfile.get_postlink_command() << endl;
            system(buildfile.get_postlink_command().c_str());
        }
        return 0;
    }
}

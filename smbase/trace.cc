// trace.cc            see license.txt for copyright and terms of use
// code for trace.h

#include "trace.h"     // this module
#include "objlist.h"   // List
#include "str.h"       // string
#include "strtokp.h"   // StrtokParse
#include "nonport.h"   // getMilliseconds()

#include <fstream>   // std::ofstream
#include <stdlib.h>    // getenv

#if !defined(DBG_INFO_ARG0) || (EXPAND(DBG_INFO_ARG0) == 0)
str::StoragePool trace_pool;
#else
str::StoragePool trace_pool(DBG_INFO_ARG0);
#endif

// auto-init
static bool inited = false;

static ObjList<str::string>* _tracers = new (trace_pool) ObjList<str::string>(DBG_INFO_ARG0_FIRST  trace_pool);

// list of active tracers, initially empty
static ObjList<str::string>& tracers = *_tracers;

// stream connected to /dev/null
std::ofstream devNullObj("/dev/null");
static std::ostream *devNull = &devNullObj;


// initialize
static void init()
{
  if (inited) {
    return;
  }

  // there's a more efficient way to do this, but I don't care to dig
  // around and find out how
  // this leaks, and now that I'm checking for them, it's a little
  // annoying...
  //devNull = new std::ofstream("/dev/null");

  inited = true;
}


void traceAddSys(str::string const &sysName)
{
  init();

  tracers.prepend(DBG_INFO_ARG0_FIRST  new (trace_pool) str::string(DBG_INFO_ARG0_FIRST  trace_pool, sysName));
}


void traceRemoveSys(str::string const &sysName)
{
  init();

  MUTATE_EACH_OBJLIST(str::string, tracers, mut) {
    if (!mut.data()->compare(sysName)) {
      mut.deleteIt();
      return;
    }
  }
  xfailure("traceRemoveSys: tried to remove system that isn't there");
}


bool tracingSys(str::string const &sysName)
{
  init();

  FOREACH_OBJLIST(str::string, tracers, iter) {
    if (!iter.data()->compare(sysName)) {
      return true;
    }
  }
  return false;
}


void traceRemoveAll()
{
  tracers.deleteAll();
}  


std::ostream &trace(str::string const &sysName)
{
  init();

  if (tracingSys(sysName)) {
    std::cout << "%%% " << sysName << ": ";
    return std::cout;
  }
  else {
    return *devNull;
  }
}


void trstr(str::string const &sysName, str::string const &traceString)
{
  trace(sysName) << traceString << std::endl;
}


std::ostream &traceProgress(int level)
{
  if ( (level == 1) ||
       (level == 2 && tracingSys("progress2")) ) {
    static long progStart = getMilliseconds();

    return trace("progress") << (getMilliseconds() - progStart) << "ms: ";
  }
  else {
    return *devNull;
  }
}


void traceAddMultiSys(str::string const &systemNames)
{
  StrtokParse tok(systemNames, ",");
  loopi(tok) {
    if (tok[i][0] == '-') {
      // treat a leading '-' as a signal to *remove*
      // a tracing flag, e.g. from some defaults specified
      // statically      
      const str::string &name = tok[i]+1;
      if (tracingSys(name)) {      // be forgiving here
        traceRemoveSys(name);
      }
      else {
        std::cout << "Currently, `" << name << "' is not being traced.\n";
      }
    }
    
    else {
      // normal behavior: add things to the trace list
      traceAddSys(tok[i]);
    }
  }
}


bool traceProcessArg(int &argc, char **&argv)
{
  traceAddFromEnvVar();

  if (argc >= 3  &&  0==strcmp(argv[1], "-tr")) {
    traceAddMultiSys(argv[2]);
    argc -= 2;
    argv += 2;
    return true;
  }
  else {
    return false;
  }
}


bool ignoreTraceEnvVar = false;

void traceAddFromEnvVar()
{
  if (ignoreTraceEnvVar) {
    return;
  }

  char const *var = getenv("TRACE");
  if (var) {
    traceAddMultiSys(var);
  }

  ignoreTraceEnvVar = true;
}


// EOF

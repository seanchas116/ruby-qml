#include "backendmain.h"

using namespace RubyQml;

extern "C" {

void rbqml_init()
{
    new BackendMain();
}

}

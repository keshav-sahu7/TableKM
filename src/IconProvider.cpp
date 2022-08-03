#include "IconProvider.hpp"

namespace theme{

char theme_path[7] = "light/";

void setThemePath(const char *theme_name)
{
    if(std::strcmp(theme_name,"dark") == 0)
        std::strcpy(theme::theme_path,"dark/");
    else
        std::strcpy(theme::theme_path,"light/");
}

}




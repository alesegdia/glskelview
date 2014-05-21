#include <iostream>
#include <cstdlib>
#include "app/flapp.h"

int main()
{
	FlApp app(1100,650,"ogl");
    app.show();
    //app.resizable(app);
    return(Fl::run());
}

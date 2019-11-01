
project(ios-app)
set(Sources AppDelegate.m ViewController.mm main.m)
add_executable(ios-app ${Sources})
target_link_libraries(ios-app vtb)



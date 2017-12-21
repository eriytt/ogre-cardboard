#include <unistd.h>
#include <ois/OIS.h>

#include "testapp.hh"

class EventHandler : public OIS::KeyListener
{
public:
  OgreCardboardTestApp *app;
  EventHandler(OgreCardboardTestApp *app) : app(app) {}
  bool keyPressed( const OIS::KeyEvent &arg ) {
    if (arg.key == OIS::KC_W)
      app->handleKeyDown(3);
    if (arg.key == OIS::KC_S)
      app->handleKeyDown(4);
    if (arg.key == OIS::KC_A)
      app->handleKeyDown(1);
    if (arg.key == OIS::KC_D)
      app->handleKeyDown(2);

    return true;
  }
  bool keyReleased( const OIS::KeyEvent &arg ) {
    if (arg.key == OIS::KC_W)
      app->handleKeyUp(3);
    if (arg.key == OIS::KC_S)
      app->handleKeyUp(4);
    if (arg.key == OIS::KC_A)
      app->handleKeyUp(1);
    if (arg.key == OIS::KC_D)
      app->handleKeyUp(2);
    return true;
  }
};

int main(int argc, char *argv[])
{
  OgreCardboardTestApp *app = new OgreCardboardTestApp();
  app->initialize();


  OIS::ParamList pl;
  std::ostringstream windowHndStr;

  windowHndStr << app->getWindowHandle();
  pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));
  pl.insert(std::make_pair(std::string("XAutoRepeatOn"), "true"));
  pl.insert(std::make_pair(std::string("x11_keyboard_grab"), "false"));
  pl.insert(std::make_pair(std::string("x11_mouse_grab"), "false"));
  pl.insert(std::make_pair(std::string("x11_mouse_hide"), "false"));
  OIS::InputManager *mInputManager = OIS::InputManager::createInputSystem(pl);

  OIS::Keyboard *k =
    static_cast<OIS::Keyboard*>(mInputManager->createInputObject(OIS::OISKeyboard, true));

  EventHandler *ehandler = new EventHandler(app);
  k->setEventCallback(ehandler);

  while (true) {
    k->capture();
    app->mainLoop();
    usleep(50000);
  }
}

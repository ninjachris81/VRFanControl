#ifndef COMMCONTROLLER_H
#define COMMCONTROLLER_H

#include <AbstractTask.h>
#include "CommHandler.h"

class CommController : public AbstractTask, public CommHandler {
public:
  CommController();

  void init();

  void update();
  
};


#endif // COMMCONTROLLER_H

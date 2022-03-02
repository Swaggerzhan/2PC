#include "../../coordinator/CoordinatorServer.h"


int main () {
  CoordinatorServer s(8888);
  s.start();
}
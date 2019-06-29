#include "Arduino.h"

#define TESTC_ID 100
#define TESTF_ID 101
#define TESTB_ID 102

/*
class Override_Tests: public TestOnce {
  protected:

    void setup() override {
      float target = 20.30;
      float hyst = 5.00;
      greenhouse.initOverride(TESTC_ID, 0, 10, 0, 12, 0, true);
      greenhouse.initOverride(TESTF_ID, 0, ABOVE, target, hyst, true);
      greenhouse.initOverride(TESTB_ID, 0, true);
    }

};


testF(Override_Tests,Override_initialized){
  assertTrue(greenhouse.isInitialized(TESTC_ID));
}

testF(Override_Tests,Override_enable){
  assertTrue(greenhouse.isEnabled(TESTC_ID));
}

testF(Override_Tests,Override_disable){
  greenhouse.disable(TESTC_ID);
  bool enable = greenhouse.isEnabled(TESTC_ID);
  greenhouse.enable(TESTC_ID);
  assertFalse(enable);
}

testF(Override_Tests,Override_Fix_true){
  greenhouse.checkOverride(TESTC_ID, H1,10, 30);
  assertTrue(greenhouse.isActive(TESTC_ID));
}

testF(Override_Tests,Override_Fix_false){
  greenhouse.checkOverride(TESTC_ID, H1, 9, 30);
  assertFalse(greenhouse.isActive(TESTC_ID));
}
testF(Override_Tests,Override_Floatrel_initialized){
  assertTrue(greenhouse.isInitialized(TESTF_ID));
}
testF(Override_Tests,Override_Floatrel_enabled){
  assertTrue(greenhouse.isEnabled(TESTF_ID));
}


testF(Override_Tests,Override_Floatrel_secondInitialization){
  float newTarget;
  float target = 20.00;
  float hyst = 5.00;
  greenhouse.initOverride(TESTF_ID, 0, ABOVE, target, hyst, true);
  newTarget = greenhouse.floatOv[greenhouse.indexPosition(TESTF_ID)].floatVar.value();

  target = 20.30;
  greenhouse.initOverride(TESTF_ID, 0, ABOVE, target, hyst, true);
  assertEqual(newTarget, 20.00);
}

testF(Override_Tests,Override_Floatrel_true1){
  float temp = 20.31;
  greenhouse.checkOverride(TESTF_ID, H1, temp);
  assertTrue(greenhouse.isActive(TESTF_ID));
}

testF(Override_Tests,Override_Floatrel_true2){
  float temp = 20.31;
  greenhouse.checkOverride(TESTF_ID, H1,temp);

  temp = 15.50;
  greenhouse.checkOverride(TESTF_ID, H1,temp);
  assertTrue(greenhouse.isActive(TESTF_ID));
}

testF(Override_Tests,Override_Floatrel_false1){
  float temp = 20.31;
  greenhouse.checkOverride(TESTF_ID, H1,temp);
  temp = 15.20;
  greenhouse.checkOverride(TESTF_ID, H1,temp);
  assertFalse(greenhouse.isActive(TESTF_ID));
}

testF(Override_Tests,Override_Floatrel_false2){
  float temp = 20.29;
  greenhouse.checkOverride(TESTF_ID,H1,temp);
  assertFalse(greenhouse.isActive(TESTF_ID));
}

testF(Override_Tests,Override_Boolrel_true){
  bool testBool = true;
  greenhouse.checkOverride(TESTB_ID,H1,testBool);
  bool testResult = greenhouse.isActive(TESTB_ID);
  assertTrue(testResult);
}

testF(Override_Tests,Override_Boolrel_false){
  bool testBool = false;
  greenhouse.checkOverride(TESTB_ID, H1, testBool);
  bool testResult = greenhouse.isActive(TESTB_ID);
  assertFalse(testResult);
}
*/

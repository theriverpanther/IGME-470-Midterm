import java.awt.AWTException;
import java.awt.Robot;
import java.awt.event.KeyEvent;
import java.io.*;
import processing.serial.*;

Serial myPort;
Robot robot;

void setup(){
  try
  {
    robot = new Robot();
  }
  catch(AWTException e)
  {
    e.printStackTrace();
  }
  myPort = new Serial(this, "COM4", 9600);

}

void draw(){
  while (myPort.available() > 0) {
    int inByte = myPort.read();
    println(inByte);
    robot.keyPress(inByte);
  }
}

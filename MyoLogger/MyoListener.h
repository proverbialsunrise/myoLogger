//
//  MyoListener.h
//  MyoLogger
//
//  Created by Daniel Johnson on 1/31/15.
//  Copyright (c) 2015 Daniel Johnson. All rights reserved.
//

#ifndef __MyoLogger__myoListener__
#define __MyoLogger__myoListener__

#include <stdio.h>
#include <myo/myo.hpp>
#include <array>

class MyoListener : public myo::DeviceListener {
    // The values of this array is set by onEmgData()
    std::array<int8_t, 8> emgSamples;
    
    // These values are set by onArmSync() and onArmUnsync()
    bool onArm;
    myo::Arm whichArm;
    
    // This is set by onUnlocked() and onLocked()
    bool isUnlocked;
    
    // These values are set by onOrientationData(), onGyroscopeData() onAccelerationData() and onPose()
    float roll, pitch, yaw;
    float omegaX, omegaY, omegaZ;
    float xAccel, yAccel, zAccel;
    myo::Pose currentPose;

    
    
    // onUnpair() is called whenever the Myo is no longer connected
    void onUnpair(myo::Myo* myo, uint64_t timestamp);
    
    // onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
    void onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg);
    
    // onAccelerometerData() is called whenever the Myo sends new accelerometer data
    void onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel);
    
    // onOrientationData() is called whenever the Myo sends new orientation data
    void onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat);
    
    // onGyroscopeData() is called whenever the Myo sends new angular velocity data.
    void onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro);
    
    // onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
    // making a fist, or not making a fist anymore.
    void onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose);
    
    // onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
    // arm. This lets Myo know which arm it's on and which way it's facing.
    void onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection);
    
    
    // onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
    // it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
    // when Myo is moved around on the arm.
    void onArmUnsync(myo::Myo* myo, uint64_t timestamp);
    
    
    // onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
    void onUnlock(myo::Myo* myo, uint64_t timestamp);
    
    
    // onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
    void onLock(myo::Myo* myo, uint64_t timestamp);
    
    
public:
    
    MyoListener();
    
    void print(float time);
    
    void printToStream(std::ostream & out, float time);
};

#endif /* defined(__MyoLogger__myoListener__) */

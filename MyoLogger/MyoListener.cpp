//
//  MyoListener.cpp
//  MyoLogger
//
//  Created by Daniel Johnson on 1/31/15.
//  Copyright (c) 2015 Daniel Johnson. All rights reserved.
//

#include "MyoListener.h"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>



MyoListener::MyoListener()
: emgSamples(), onArm(false), isUnlocked(false), roll(0), pitch(0), yaw(0), xAccel(0), yAccel(0), zAccel(0), currentPose() {
}

// onUnpair() is called whenever the Myo is disconnected from Myo Connect by the user.
void MyoListener::onUnpair(myo::Myo* myo, uint64_t timestamp)
{
    // We've lost a Myo.
    // Let's clean up some leftover state.
    emgSamples.fill(0);
    roll = 0;
    pitch = 0;
    yaw = 0;
    xAccel = 0;
    yAccel = 0;
    zAccel = 0;
    onArm = false;
    isUnlocked = false;
}

// onEmgData() is called whenever a paired Myo has provided new EMG data, and EMG streaming is enabled.
void MyoListener::onEmgData(myo::Myo* myo, uint64_t timestamp, const int8_t* emg)
{
    time = timestamp;
    for (int i = 0; i < 8; i++) {
        emgSamples[i] = emg[i];
    }
}

void MyoListener::onAccelerometerData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& accel)
{
    float g = 9.81;
    xAccel = accel.x()*g;
    yAccel = accel.y()*g;
    zAccel = accel.z()*g;
    
}

void MyoListener::onOrientationData(myo::Myo* myo, uint64_t timestamp, const myo::Quaternion<float>& quat)
{
    using std::atan2;
    using std::asin;
    using std::sqrt;
    using std::max;
    using std::min;
    
    // Calculate Euler angles (roll, pitch, and yaw) from the unit quaternion.
    roll = atan2(2.0f * (quat.w() * quat.x() + quat.y() * quat.z()),
                 1.0f - 2.0f * (quat.x() * quat.x() + quat.y() * quat.y()));
    pitch = asin(max(-1.0f, min(1.0f, 2.0f * (quat.w() * quat.y() - quat.z() * quat.x()))));
    yaw = atan2(2.0f * (quat.w() * quat.z() + quat.x() * quat.y()),
                1.0f - 2.0f * (quat.y() * quat.y() + quat.z() * quat.z()));
    
    
}

void MyoListener::onGyroscopeData(myo::Myo* myo, uint64_t timestamp, const myo::Vector3<float>& gyro) {
    omegaX = gyro.x();
    omegaY = gyro.y();
    omegaZ = gyro.z();
}


// onPose() is called whenever the Myo detects that the person wearing it has changed their pose, for example,
// making a fist, or not making a fist anymore.
void MyoListener::onPose(myo::Myo* myo, uint64_t timestamp, myo::Pose pose)
{
    currentPose = pose;
    
    if (pose != myo::Pose::unknown && pose != myo::Pose::rest) {
        // Tell the Myo to stay unlocked until told otherwise. We do that here so you can hold the poses without the
        // Myo becoming locked.
        myo->unlock(myo::Myo::unlockHold);
        
        // Notify the Myo that the pose has resulted in an action, in this case changing
        // the text on the screen. The Myo will vibrate.
        myo->notifyUserAction();
    } else {
        // Tell the Myo to stay unlocked only for a short period. This allows the Myo to stay unlocked while poses
        // are being performed, but lock after inactivity.
        myo->unlock(myo::Myo::unlockTimed);
    }
}

// onArmSync() is called whenever Myo has recognized a Sync Gesture after someone has put it on their
// arm. This lets Myo know which arm it's on and which way it's facing.
void MyoListener::onArmSync(myo::Myo* myo, uint64_t timestamp, myo::Arm arm, myo::XDirection xDirection)
{
    std::cout << "Arm Synced!" << std::endl;
    onArm = true;
    whichArm = arm;
}

// onArmUnsync() is called whenever Myo has detected that it was moved from a stable position on a person's arm after
// it recognized the arm. Typically this happens when someone takes Myo off of their arm, but it can also happen
// when Myo is moved around on the arm.
void MyoListener::onArmUnsync(myo::Myo* myo, uint64_t timestamp)
{
    onArm = false;
}

// onUnlock() is called whenever Myo has become unlocked, and will start delivering pose events.
void MyoListener::onUnlock(myo::Myo* myo, uint64_t timestamp)
{
    isUnlocked = true;
}

// onLock() is called whenever Myo has become locked. No pose events will be sent until the Myo is unlocked again.
void MyoListener::onLock(myo::Myo* myo, uint64_t timestamp)
{
    isUnlocked = false;
}


// We define this function to print the current values that were updated by the on...() functions above.
void MyoListener::print()
{
    // Clear the current line
    std::cout << '\r';
    std::cout << "Timestamp: " << time << std::endl;
    std::cout << "EMG Data" << std::endl;
    // Print out the EMG data.
    for (size_t i = 0; i < emgSamples.size(); i++) {
        std::ostringstream oss;
        oss << static_cast<int>(emgSamples[i]);
        std::string emgString = oss.str();
        std::cout <<  emgString << ',';
    }
    std::cout << std::endl;
    
    //Print out the roll data.
    std::cout << "Orientation Data" << std::endl;
    std::cout << roll << ',' << pitch << ',' << yaw << std::endl;
    
    std::cout << "Angular Velocity Data" << std::endl;
    std::cout << omegaX << ',' << omegaY << ',' << omegaZ << std::endl;
    
    //Print out acceleration data.
    std::cout << "Acceleration Data" << std::endl;
    std::cout << xAccel << ',' << yAccel << ',' << zAccel << std::endl;
    
    //Print out current pose.
    std::string poseString = currentPose.toString();
    std::cout << poseString << std::endl;
    
    std::cout << std::flush;
}

void MyoListener::printToStream(std::ostream & out)
{
    //EMG Data
    out << time << ',';
    for (size_t i = 0; i < emgSamples.size(); i++) {
        std::ostringstream oss;
        oss << static_cast<int>(emgSamples[i]);
        std::string emgString = oss.str();
        out <<  emgString << ',';
    }
    //Orientation Data
    out << roll << ',' << pitch << ',' << yaw << ',';
    //Angular velocity Data
    out << omegaX << ',' << omegaY << ',' << omegaZ << ',';
    //Acceleration Data
    out << xAccel << ',' << yAccel << ',' << zAccel << ',';
    //Pose Data
    std::string poseString = currentPose.toString();
    out << poseString << std::endl;
}


//
//  main.cpp
//  MyoLogger
//
//  Created by Daniel Johnson on 1/31/15.
//  Copyright (c) 2015 Daniel Johnson. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <myo/myo.hpp>
#include "myoListener.h"

#include <boost/program_options.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace po = boost::program_options;
namespace ptime = boost::posix_time;

int main(int argc, const char * argv[]) {
    //Parse arguments
    std::string outputFile;
    float runtime;
    
    po::options_description desc("Allowed options");
    desc.add_options()
    ("help", "Produce help message")
    ("output-file", po::value<std::string>(&outputFile)->default_value(std::string("testFile.csv")), "file to save results to")
    ("runtime", po::value<float>(&runtime)->default_value(-1.0), "Record data for <runtime> seconds")
    ;
    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    //Print out the help text
    if (vm.count("help")) {
        std::cout << desc << "\n";
        return 1;
    }
    
    
    if (vm.count("output-file")) {
        std::cout << "Outputting log to: " << vm["output-file"].as<std::string>() << std::endl;
    }
    

    if (runtime > 0) {
        std::cout << "Running for " << vm["runtime"].as<float>() << " seconds." << std::endl;
    }

    
    
    
    // We catch any exceptions that might occur below -- see the catch statement for more details.
    try {
        
        // First, we create a Hub with our application identifier. Be sure not to use the com.example namespace when
        // publishing your application. The Hub provides access to one or more Myos.
        myo::Hub hub("com.example.emg-data-sample");
        
        std::cout << "Attempting to find a Myo..." << std::endl;
        
        // Next, we attempt to find a Myo to use. If a Myo is already paired in Myo Connect, this will return that Myo
        // immediately.
        // waitForMyo() takes a timeout value in milliseconds. In this case we will try to find a Myo for 10 seconds, and
        // if that fails, the function will return a null pointer.
        myo::Myo* myo = hub.waitForMyo(10000);
        
        // If waitForMyo() returned a null pointer, we failed to find a Myo, so exit with an error message.
        if (!myo) {
            throw std::runtime_error("Unable to find a Myo!");
        }
        
        // We've found a Myo.
        std::cout << "Connected to a Myo armband!" << std::endl << std::endl;
        
        // Next we enable EMG streaming on the found Myo.
        myo->setStreamEmg(myo::Myo::streamEmgEnabled);
        
        // Next we construct an instance of our DeviceListener, so that we can register it with the Hub.
        MyoListener listener;
        
        // Hub::addListener() takes the address of any object whose class inherits from DeviceListener, and will cause
        // Hub::run() to send events to all registered device listeners.
        hub.addListener(&listener);
        
        
        //Open the log file. Setup headers
        std::ofstream logfile;
        logfile.open(outputFile);
        logfile << "TIME,EMG[0],EMG[1],EMG[2],EMG[3],EMG[4],EMG[5],EMG[6],EMG[7],ROLL,PITCH,YAW,OMEGAX,OMEGAY,OMEGAZ,XACCEL,YACCEL,ZACCEL,POSE" << std::endl;
        
        
        //Determine if we're running the listener loop for a certain amount of time. If so, setup the necessary variables to handle that.
        bool runForTime = false;
        ptime::ptime startTime;
        if (runtime > 0) {
            runForTime = true;
            startTime = ptime::microsec_clock::local_time();
        }
        

        ptime::ptime currentTime = ptime::microsec_clock::local_time();
        ptime::time_duration elapsedTime = currentTime - startTime;
        
        
        while (!(runForTime) || elapsedTime.total_milliseconds() <  runtime * 1000.0) {
            // In each iteration of our main loop, we run the Myo event loop for a set number of milliseconds.
            // In this case, we wish to update our display 50 times a second, so we run for 1000/20 milliseconds.
            hub.run(1000/20);
            
            //Update the time checks
            currentTime = ptime::microsec_clock::local_time();
            elapsedTime = currentTime - startTime;
            // After processing events, call the print() member function we defined above to print out the values we've
            // obtained from any events that have occurred. Then print to the log file.
            listener.print(elapsedTime.total_milliseconds());
            listener.printToStream(logfile, elapsedTime.total_milliseconds());
        }
        
        // If a standard exception occurred, we print out its message and exit.
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        std::cerr << "Press enter to continue.";
        std::cin.ignore();
        return 1;
    }

}

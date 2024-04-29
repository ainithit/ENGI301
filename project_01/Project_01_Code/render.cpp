 /*
 ____  _____ _        _
| __ )| ____| |      / \
|  _ \|  _| | |     / _ \
| |_) | |___| |___ / ___ \
|____/|_____|_____/_/   \_\
http://bela.io
*/
/**
\example Audio/file-player/render.cpp

Playback of several large wav files
---------------------------

This example shows how to play audio files from a playlist sequentially.
It uses the AudioFileReader class, which loads data from the audio file into
memory in the background, while the user can request data safely from the
RT-thread via the AudioFileReader::getSamples() method.

We want to start playing back the next file as soon as the current one
finishes, so we need to start preloading the next one ahead of time. This is
done by using two AudioFileReader objects, where at any time one, whose index
is `gCurrReader`, will be playing back the current file while the other one
starts preloading the next file.
*/

#include <Bela.h>
#include <libraries/AudioFile/AudioFile.h>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>

// Initializing reader, songs, buttons, flags, and the requirements for rendering.
std::vector<AudioFileReader> gReaders(2);
std::vector<std::string> gFilenames = {
	// Insert song file names here:
	// "number1.wav",
	// "number2.wav",
	// "number3.wav",
	// "number4.wav",
	// "number5.wav",
	// "number6.wav",
	// "number7.wav",
};
size_t gLoadingFile;
size_t gCurrReader;

std::vector<float> gSamples;
size_t gFrameCount = 0;
AuxiliaryTask gStartLoadingFileTask;
bool bela_on = true;

float volume = 1;

// defining button GPIO
#define PLAY_PAUSE_BUTTON_PIN   "/sys/class/gpio/gpio59/value";
#define SKIP_BUTTON_PIN   "/sys/class/gpio/gpio58/value";
#define REPLAY_BUTTON_PIN "/sys/class/gpio/gpio57/value";
#define VOLUME_UP_PIN   "/sys/class/gpio/gpio60/value";
#define VOLUME_DOWN_PIN "/sys/class/gpio/gpio52/value";

std::atomic<bool> gPauseFlag(false); // Flag to indicate whether the song is paused
std::atomic<bool> gSkipFlag(false); 
std::atomic<bool> gPrevFlag(false); 

// Function to read the buttons' state asynchronously
void readPlayPauseButtonState() {
    std::ifstream playPauseButton("/sys/class/gpio/gpio59/value");
    if (!playPauseButton.is_open()) {
        std::cerr << "Failed to open play/pause button GPIO file." << std::endl;
        return;
    }
    std::ifstream skipButton("/sys/class/gpio/gpio58/value");
    if (!skipButton.is_open()) {
        std::cerr << "Failed to open skip button GPIO file." << std::endl;
        return;
    }
    std::ifstream prevButton("/sys/class/gpio/gpio57/value");
    if (!prevButton.is_open()) {
        std::cerr << "Failed to open prev button GPIO file." << std::endl;
        return;
    }
    
        std::ifstream volumeUpButton("/sys/class/gpio/gpio60/value");
    if (!prevButton.is_open()) {
        std::cerr << "Failed to open volume up button GPIO file." << std::endl;
        return;
    }
    
        std::ifstream volumeDownButton("/sys/class/gpio/gpio52/value");
    if (!prevButton.is_open()) {
        std::cerr << "Failed to open volume down button GPIO file." << std::endl;
        return;
    }
    
    std::string play_state;
    std::string prev_play_state;
    
    std::string skip_state;
    std::string prev_skip_state;
    
    std::string prev_state;
    std::string prev_prev_state;
    
    std::string volume_up_state;
    std::string prev_volume_up_state;
    
    std::string volume_down_state;
    std::string prev_volume_down_state;
    
    while (bela_on) {
        // Read the play/pause button state
        std::getline(playPauseButton, play_state);
        // Read the skip button state
        std::getline(skipButton, skip_state);
        // Read the prev button state
        std::getline(prevButton, prev_state);
        // Read the volume up button state
        std::getline(volumeUpButton, volume_up_state);        
        // Read the volume down button state
        std::getline(volumeDownButton, volume_down_state); 
        
        // Check if the pause/play state has changed since the last iteration
        if (prev_play_state == "0" && play_state == "1") {
            // Toggle the pause flag based on the button state
            std::cerr << "PAUSE/PLAY Button pressed" << std::endl;
            gPauseFlag.store(!gPauseFlag.load(std::memory_order_relaxed), std::memory_order_relaxed);

        }
        
        // Check if the skip state has changed since the last iteration
        if (prev_skip_state == "0" && skip_state == "1") {
            // Toggle the pause flag based on the button state
            std::cerr << "SKIP Button pressed" << std::endl;
            gSkipFlag.store(!gSkipFlag.load(std::memory_order_relaxed), std::memory_order_relaxed);

        }
        
        // Check if the skip state has changed since the last iteration
        if (prev_prev_state == "0" && prev_state == "1") {
            // Toggle the pause flag based on the button state
            gPrevFlag.store(!gPrevFlag.load(std::memory_order_relaxed), std::memory_order_relaxed);
            std::cerr << "PREV Button pressed" << std::endl;

        }
        
        // Check if the volume up state has changed since the last iteration
        if (prev_volume_up_state == "0" && volume_up_state == "1") {
            // Toggle the pause flag based on the button state
            if (volume != 1) {
            	volume += 0.05;
            }
            std::cerr << "VOLUME UP Button pressed" << std::endl;

        }
        
        // Check if the volume down state has changed since the last iteration
        if (prev_volume_down_state == "0" && volume_down_state == "1") {
            // Toggle the pause flag based on the button state
            if (volume != 0) {
            	volume -= 0.05;
            }
            std::cerr << "VOLUME DOWN Button pressed" << std::endl;

        }
        
        prev_play_state = play_state;
        prev_skip_state = skip_state;
        prev_prev_state = prev_state;
        prev_volume_up_state = volume_up_state;
        prev_volume_down_state = volume_down_state;     
        
        playPauseButton.clear(); // Clear any error flags
        playPauseButton.seekg(0, std::ios::beg); // Move the get pointer to the beginning of the file
        skipButton.clear(); // Clear any error flags
        skipButton.seekg(0, std::ios::beg); // Move the get pointer to the beginning of the file
        prevButton.clear(); // Clear any error flags
        prevButton.seekg(0, std::ios::beg); // Move the get pointer to the beginning of the file
        volumeUpButton.clear(); // Clear any error flags
        volumeUpButton.seekg(0, std::ios::beg); // Move the get pointer to the beginning of the file
        volumeDownButton.clear(); // Clear any error flags
        volumeDownButton.seekg(0, std::ios::beg); // Move the get pointer to the beginning of the file
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust sleep duration as needed
    }
    // Close the file
    playPauseButton.close();
}

// Starts a thread that runs the function that reads the buttons state
std::thread buttonThread(readPlayPauseButtonState);

// Loads the next file for real-time audio programming
void loadNextFile(void*)
{
	// start preloading the next file
	gLoadingFile = (gLoadingFile + 1) % gFilenames.size();
	size_t nextReader = (gCurrReader + 1) % gReaders.size();
	gReaders[nextReader].setup(gFilenames[gLoadingFile], 16384);
	rt_printf("Opening file [%d] %s in reader %d\n", gLoadingFile, gFilenames[gLoadingFile].c_str(), nextReader);
}

// Sets up variables before rendering
bool setup(BelaContext *context, void *userData)
{
	// create a task to load files
	gStartLoadingFileTask = Bela_createAuxiliaryTask(loadNextFile, 1, "loadNextFile");
	if(!gStartLoadingFileTask) {
		fprintf(stderr, "Error creating file loading task\n");
		return false;
	}
	gLoadingFile = -1;
	gCurrReader = -1;
	// open the first file
	loadNextFile(NULL);
	gCurrReader = 0;
	// open the second file
	loadNextFile(NULL);
	gSamples.reserve(context->audioFrames * gReaders[gCurrReader].getChannels());
	
	std::string line;
	
	return true;
}

// Renders the program
void render(BelaContext *context, void *userData) {
    AudioFileReader& reader = gReaders[gCurrReader];
    gSamples.resize(context->audioFrames * reader.getChannels());
    reader.getSamples(gSamples);

    for (unsigned int n = 0; n < context->audioFrames; ++n) {
        float out;
        
        // If paused, send zeros to the output
        if (gPauseFlag.load(std::memory_order_relaxed)) {
            for(unsigned int c = 0; c < context->audioOutChannels; ++c) {
                audioWrite(context, n, c, 0.0f);
            }
            continue; // Move to the next frame
        }

        // Process audio samples from the file
        for(unsigned int c = 0; c < context->audioOutChannels; ++c) {
            if(c < reader.getChannels()) {
                out = gSamples[n * reader.getChannels() + c];
            } else {
                // If there are more output channels than channels in the file,
                // copy the file's last channel to all remaining outputs
                out = gSamples[(reader.getLength() - 1) * reader.getChannels() + c];
            }
            // Volume variable allows for change in volume
            audioWrite(context, n, c, out * volume);
        }
    }
    
    // Logic for going to the previous song
    if (gPrevFlag.load(std::memory_order_relaxed)) {
    	gFrameCount = 0;
    	// Switch to the same file
        gCurrReader = (gCurrReader + (gReaders.size() - 1)) % gReaders.size();
        reader.getSamples(gSamples);
        rt_printf("Playing from reader: %d\n", gCurrReader);
        gPrevFlag.store(false, std::memory_order_relaxed);
        // Start loading the next file
        Bela_scheduleAuxiliaryTask(gStartLoadingFileTask);
    }

    // Load the next song for skip and checks if the current file has finished playing
    if(gFrameCount >= reader.getLength() || gSkipFlag.load(std::memory_order_relaxed)) {
        gFrameCount = 0;
        // Switch to the next file
        gCurrReader = (gCurrReader + 1) % gReaders.size();
        reader.getSamples(gSamples);
        rt_printf("Playing from reader: %d\n", gCurrReader);
        gSkipFlag.store(false, std::memory_order_relaxed);
        // Start loading the next file
        Bela_scheduleAuxiliaryTask(gStartLoadingFileTask);
    } else {
        gFrameCount += context->audioFrames;
    }
}

// At the end of the program, this is used to ensure that bela will shut down correctly.
void cleanup(BelaContext *context, void *userData)
{
	// Breaks while loop within the thread that reads the button states
	bela_on = false;
	// Ends the thread
	buttonThread.join();
}
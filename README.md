<img width="602" height="426" alt="Screenshot 2026-08-30 at 12 17 58 AM" src="https://github.com/user-attachments/assets/1a9c0fd1-f75c-4f21-91b6-447de242a5bf" />

Convolution Reverb is an audio plugin for real-time convolution reverb. It mimics realistic acoustic spaces by convolving your track's incoming audio with an impulse response (IR), which is the recorded reverberation of a real room or space, captured from anywhere a user chooses. Just load your own IR file directly into the plugin to recreate the reverb of any space someone has recorded and shared, with real-time control over dry/wet mix and gain.

Built in C++ with the JUCE framework, Convolution Reverb uses partitioned FFT-based convolution for efficient real-time processing, with thread-safe parameter handling between the UI and audio engine.

Getting IRs: Convolution Reverb uses user-loaded impulse responses, so you'll need to get your own .wav IR file via the plugin's file browser. Here's a few good free sources:
- OpenAIR (www.openairlib.net): A collective library of various acoustic-space IRs from the University of York
  - as of August 29, 2026, the OpenAIR website is down and has been down for some time
- EchoThief (www.echothief.com): A library of real-space IRs recorded in places across North America, free for any use, including commercial

Convolution Reverb is an open-source plugin under the MIT license, meaning you can use, modify, and even distribute your own version of the source code with very few restrictions. And it's free to download and use!

Features:
- Real-time convolution reverb via impulse response loading
- Dry/wet mix control
- Output gain control
- Load any user-supplied .wav impulse response at runtime

**Requires JUCE and a C++ compiler supporting VST3/AU plugin targets. Clone this repo, open the project in the Projucer (or your IDE), and build the VST3/AU target for your platform.

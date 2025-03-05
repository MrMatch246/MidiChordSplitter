Small Midi Chord Splitter.
whatever (high,mid,low) is turned on (all are by default) will be passed thru.
you can use the knobs to set the offset so that you can split a 4 notes chord (up to 3*16 notes)
Put some of them in an instrument rack and configure them and then recieve the notes somewhere else(there is no concept of a pure midi effect in vst sadly)

Oh and you will have to compile it yourself for now. super easy. just Download JUCE, set the add_subdirectory path in CMakeLists.txt and build. 

if there is interest for a release/precompiled write an issue and ill do it.

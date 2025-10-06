# Simplified Dual Waveshape Distortion

The initial implementation was too complex for the current framework. 

## Current Framework Limitations
- No AudioProcessorValueTreeState support yet
- No attachment system for automatic parameter sync
- Parameters accessed directly through ParameterManager

## Simplified Version
Creating a minimal dual distortion plugin that:
- Uses 2 distortion channels (A & B)
- Simple LFO modulation  
- Direct parameter access (no attachments)
- Manual slider-to-parameter connection

This matches the SimpleDistortion example pattern.

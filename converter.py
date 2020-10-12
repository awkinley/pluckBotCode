import midi

# MIDI notes are defined using an integer value, 
# this converts from that integer value to a Hz value
def noteToFreq(noteNum):
	return 440 * pow(2, (noteNum - 69)/12.0)

# Our PWM timer uses ACLK which has a frequency of 32768
# ACLK / note freq gives the number of clock cycles for the note
def freqToTicks(freq):
	return 32768 / freq;

def noteToLED(note):
	if(note > 70):
		return "LED1"
	else:
		return "LED2"

# these values can be changed to fit each individual song
# try to generaly seperate note values by pitch among the
# 4 input buttons
def noteToKeypad(note):
	if(note >= 72):
		return 4
	elif(note >= 68):
		return 3
	elif(note >= 64):
		return 2
	else:
		return 1

# read in the file
pattern = midi.read_midifile("Marry_Had_A_Little_Lamb.mid")
# get the track data
track = pattern[0]

last_note = -1

# could be read from midi file but this is simpler and allows me to slow down songs if necessary
tempo = 60.0
secondsPerBeat = 60.0 / tempo

# there are usually 480 midi 'ticks' in a beat of the song
ticksPerBeat = 480.0
miliSecondsPerTick = (secondsPerBeat / ticksPerBeat) * 1000.0
# the array is made up of struct definitions
formatString = "{{ {}, {}}}, "

#print the starting curly bracket of the array
print("{")
#count the number of notes in the song
count = 0
for event in track:
	if isinstance(event, midi.NoteOnEvent):
		data = event.data
		note = data[0]
		velocity = data[1]

		# velocity > 0 means a note starts
		if velocity > 0:
			last_note = note
			ticksElapsed = event.tick #time between the last note end and this note starting
			msElapsed = ticksElapsed * miliSecondsPerTick
			# my music transcription software like to add short pauses between sequential notes
			# we ignore them unless they are sufficiently long to actually be a rest
			if(msElapsed > 150):
				# a negative pitch interval means a rest
				print(formatString.format(-1, int(msElapsed), 0))
				count += 1
		# if this not velocity is 0 that means the last note has ended
		if velocity == 0:
			if note != last_note: # if it's trying to end anything other than the previous note is started, error
				print("there was an issue, too many notes trying to play")
			else:
				# get the number of ACLK cyles that represent the 
				# pitch of the note (the MAX_CNT of timer B0)
				# pitchTicks = freqToTicks(noteToFreq(note))
#
				# amount of time the note plays for
				ticksElapsed = event.tick
				msElapsed = ticksElapsed * miliSecondsPerTick

				#round these to the closest integer
				# pitchTicks = round(pitchTicks, 0)
				msElapsed = round(msElapsed)

				keypad = noteToKeypad(note)
				# print out the c representation of the array element for this note
				print(formatString.format(note, int(msElapsed)))
				count += 1
print("}")
# print out the number of notes (for the array definition)
print("count: {}".format(count))


from collections import namedtuple
import midi

Note = namedtuple('Note', 'note length_ms')

def midiToNotes(filename, tempo):
	notes = []

	# read in the file
	pattern = midi.read_midifile(filename)
	# get the track data
	track = pattern[0]
	print(pattern)
	last_note = -1

	# could be read from midi file but this is simpler and allows me to slow down songs if necessary
	# tempo = 120.0
	secondsPerBeat = 60.0 / tempo

	# there are usually 480 midi 'ticks' in a beat of the song
	ticksPerBeat = 480.0
	miliSecondsPerTick = (secondsPerBeat / ticksPerBeat) * 1000.0

	for event in track:
		if isinstance(event, midi.NoteOnEvent) or isinstance(event, midi.NoteOffEvent) :
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
					newNote = Note(-1, int(msElapsed))
					notes.append(newNote)

			# if this not velocity is 0 that means the last note has ended
			if velocity == 0 or isinstance(event,midi.NoteOffEvent):
				if note != last_note: # if it's trying to end anything other than the previous note is started, error
					print("there was an issue, too many notes trying to play")
				else:
					# amount of time the note plays for
					ticksElapsed = event.tick
					msElapsed = ticksElapsed * miliSecondsPerTick

					#round these to the closest integer
					# pitchTicks = round(pitchTicks, 0)
					msElapsed = round(msElapsed)

					# print out the c representation of the array element for this note
					newNote = Note(note, int(msElapsed))
					notes.append(newNote)
	return notes


note_array_format = "Note {}[{}] = "
note_format = "{{ {}, {} }}"
song_format = "Song {name}Song = {{\"{name}\", {name}, {length}}};"

def createHeaderFile(notes, filename):
	file = open("pluckBot/" + filename, 'w')
	# create note array
	array_name = filename[:len(filename) - 2]
	note_array = note_array_format.format(array_name, len(notes))
	file.write(note_array)

	note_strings = [note_format.format(n.note, n.length_ms) for n in notes]
	notes_string = "{\n"
	notes_string += ',\n'.join(note_strings)
	notes_string += "\n};"
	file.write(notes_string)

	# create song definition
	song_string = song_format.format(name = array_name, length = len(notes))
	file.write("\n\n\n")
	file.write(song_string)
	

include_format = "#include \"{}\""
def main():
	midi_filename = input("name of midi file: ")
	tempo = int(input("what tempo would you like the song at: "))
	notes = midiToNotes(midi_filename, tempo)
	output_filename = input("what would you like to call the output file (probably songname.h). This will overwrite any existing file with that name. Make sure that it can also be a valid variable declaration in C: ")
	if output_filename[-2:] != ".h":
		print("the file name you gave is not a .h, this is not allowed")
		return
	print(notes)
	createHeaderFile(notes, output_filename)

	print("the file has been created!\n")

	print("at the bottom of Music.h put:")
	print(include_format.format(output_filename))
	print("you probably also have to add the song to whatever list of songs are contained within the main file, the Song struct name is \"" + output_filename[:len(output_filename) - 2] + "Song" + "\"")


if __name__ == "__main__":
	main()

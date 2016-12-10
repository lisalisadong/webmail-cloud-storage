### Cache:
	- get(row, col): return string (call read)
	- put(row, col, val):
	- cput(row, col, val1, val2):
	- delete(row, col):
	- writeSnapshot() 


### FileSystem:
	- serialize(row, col, val):
	- deserialize(string): return null
	- read_file(filename): return <row, <col, val>>
	- write_file(filename, <row, <col, val>>): return 0 if success, 1 if error
	- write_entry(row, col, val): 
	- delete_entry(row, col):
	- writeMeta(row, col, file): 
	- keys_to_file(row, col): filename
	- init(keysToFile, filesToKeys)

### utils:
	- log(row, col, val, operation)
	- replay(filesystem);
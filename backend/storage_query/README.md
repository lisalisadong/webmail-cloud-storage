### Cache:
	- get(row, col): return string (call read)
	- put(row, col, val):
	- cput(row, col, val1, val2):
	- delete(row, col):
	- writeSnapshot() 


### FileSystem:
	- serialize(row, col, val):
	- deserialize(string): return null
	- read(row, col): return <row, <col, val>>
	- writeData(row, col, val): 
	- writeMeta(row, col, file): 

### utils:
	- log(row, col, val)
	- replay();






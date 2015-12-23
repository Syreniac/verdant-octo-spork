f = open("debug_output_alloc.txt","r")

find = 0x000000000702C3B0

class MemHeap:
	def __init__(self):
		self.array = []
	
	def add_mem(self,string):
		self.array.append(MemBlock(string[0:6],string))

	def remove_mem(self,string):
		i = 0
		while(i < len(self.array)):
			freeAddress = string
			self.array[i].test_free()
			i = i+1
			
	def test_overlaps(self):
		i = j = 0
		while(i < len(self.array)):
			j = 0
			while(j < len(self.array)):
				if(i != j):
					b = self.array[i].test_overlap(self.array[j])
					if(b):
						print "colliding blocks "+str(self.array[i])+" and "+str(self.array[j])
				j += 1
				
			i+=1
class MemBlock:
	def __init__(self, type, string):
		split_lines = string.split(" ");
		print type
		if(type == "reallo"):
			self.start = int(split_lines[8],16)
			self.end = ((int(split_lines[8],16) + int(split_lines[11]) * 8))
		else:
			self.start = int(split_lines[5],16)
			self.end = ((int(split_lines[5],16) + int(split_lines[8]) * 8))
		self.string = string
	def test_free(self,free):
		if(self.start == free):
			self.start = 0;
			self.end = 0;
			return 1
		elif(self.start < free < self.end):
			self.start = 0
			self.end = 0
			return 2
		else:
			return 0
	def test_overlap(self,test):
		if(self.start < test.start and self.end > test.start):
			return 1
		if(self.end > test.end and self.start < test.end):
			return 1
		return 0
		
	def __str__(self):
		return "%X -> %X" % (self.start,self.end)
	def __repr__(self):
		return self.__str__

heap = MemHeap()
		
for line in f:
	heap.add_mem(line)
	
heap.test_overlaps()
	
raw_input("")
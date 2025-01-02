# My implementation for a priority queue using an array

class MinArray:
    
    def __init__(self):
        self.queue = []
        self.position = {}
    
    def insert(self, node, cost):
        if node in self.position:
            index = self.position[node]
            self.queue[index] = (node, cost)
        else:
            self.queue.append((node, cost))
            self.position[node] = len(self.queue) - 1
        
    def get_min_node(self):
        if not self.queue:
            raise IndexError("Priority queue is empty.")
    
        min_index = 0
        for i in range(1, len(self.queue)):
            if self.queue[i][1] < self.queue[min_index][1]:
                min_index = i

        self.queue[min_index], self.queue[-1] = self.queue[-1], self.queue[min_index]

        self.position[self.queue[min_index][0]] = min_index
        self.position[self.queue[-1][0]] = len(self.queue) - 1

        node, cost = self.queue.pop()
        del self.position[node]
        
        return node, cost
            
    def is_empty(self):
        return len(self.queue) == 0
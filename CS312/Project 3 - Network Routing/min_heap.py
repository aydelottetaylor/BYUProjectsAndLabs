# My Implementation of A Priority Queue using a heap

class MinHeap:
    
    def __init__(self):
        self.heap = []
        
    def insert(self, node, cost):
        self.heap.append((node, cost))
        
        self.sift_node_up(len(self.heap) - 1)
        
    def get_min(self):
        if len(self.heap) == 1:
            return self.heap.pop()
        
        min = self.heap[0]
        
        self.heap[0] = self.heap.pop()
        
        self.sift_node_down(0)
        
        return min
    
    def sift_node_up(self, index):
        parent = (index - 1) // 2
        
        while index > 0 and self.heap[index][1] < self.heap[parent][1]:
            self.heap[index], self.heap[parent] = self.heap[parent], self.heap[index]
            
            index = parent
            
            parent = (index - 1) // 2
    
    def sift_node_down(self, index):
        left = 2 * index + 1
        
        right = 2 * index + 2
        
        smallest = index
        
        if left < len(self.heap) and self.heap[left][1] < self.heap[smallest][1]:
            smallest = left
        
        if right < len(self.heap) and self.heap[right][1] < self.heap[smallest][1]:
            smallest = right
        
        if smallest != index:
            self.heap[index], self.heap[smallest] = self.heap[smallest], self.heap[index]
            
            self.sift_node_down(smallest)
            
            
    def heap_is_empty(self):
        return len(self.heap) == 0
from min_heap import MinHeap
from min_array import MinArray

def find_shortest_path_with_heap(
        graph: dict[int, dict[int, float]],
        source: int,
        target: int
) -> tuple[list[int], float]:
    """
    Find the shortest (least-cost) path from `source` to `target` in `graph`
    using the heap-based algorithm.

    Return:
        - the list of nodes (including `source` and `target`)
        - the cost of the path
    """
    distance = {node: float('inf') for node in graph}
    distance[source] = 0
    
    previous = {node: None for node in graph}
    
    priority = MinHeap()
    priority.insert(source, 0)
    
    while not priority.heap_is_empty():
        current_node, current_distance = priority.get_min()
        
        if current_node == target: break
        
        for neighbor, weight in graph[current_node].items():
            alt = current_distance + weight
            if alt < distance[neighbor]:
                distance[neighbor] = alt
                previous[neighbor] = current_node
                priority.insert(neighbor, alt)
                
    path = []
    node = target
    while node is not None:
        path.append(node)
        node = previous[node]
    
    path.reverse()
    
    return path, distance[target]    


def find_shortest_path_with_array(
        graph: dict[int, dict[int, float]],
        source: int,
        target: int
) -> tuple[list[int], float]:
    """
    Find the shortest (least-cost) path from `source` to `target` in `graph`
    using the array-based (linear lookup) algorithm.

    Return:
        - the list of nodes (including `source` and `target`)
        - the cost of the path
    """

    distance = {node: float('inf') for node in graph}
    distance[source] = 0
    
    previous = {node: None for node in graph}
    
    priority = MinArray()
    priority.insert(source, 0)
    
    while not priority.is_empty():
        current_node, current_distance = priority.get_min_node()
        
        if current_node == target: break
        
        for neighbor, weight in graph[current_node].items():
            alt = current_distance + weight
            if alt < distance[neighbor]:
                distance[neighbor] = alt
                previous[neighbor] = current_node
                priority.insert(neighbor, alt)
                
    path = []
    node = target
    while node is not None:
        path.append(node)
        node = previous[node]
        
    path.reverse()
    
    return path, distance[target]
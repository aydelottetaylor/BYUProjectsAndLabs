
import math
import random
import heapq

from tsp_core import Tour, SolutionStats, Timer, score_tour, Solver
from tsp_cuttree import CutTree


def random_tour(edges: list[list[float]], timer: Timer) -> list[SolutionStats]:
    stats = []
    n_nodes_expanded = 0
    n_nodes_pruned = 0
    cut_tree = CutTree(len(edges))

    while True:
        if timer.time_out():
            return stats

        tour = random.sample(list(range(len(edges))), len(edges))
        n_nodes_expanded += 1

        cost = score_tour(tour, edges)
        if math.isinf(cost):
            n_nodes_pruned += 1
            cut_tree.cut(tour)
            continue

        if stats and cost > stats[-1].score:
            n_nodes_pruned += 1
            cut_tree.cut(tour)
            continue

        stats.append(SolutionStats(
            tour=tour,
            score=cost,
            time=timer.time(),
            max_queue_size=1,
            n_nodes_expanded=n_nodes_expanded,
            n_nodes_pruned=n_nodes_pruned,
            n_leaves_covered=cut_tree.n_leaves_cut(),
            fraction_leaves_covered=cut_tree.fraction_leaves_covered()
        ))

    if not stats:
        return [SolutionStats(
            [],
            math.inf,
            timer.time(),
            1,
            n_nodes_expanded,
            n_nodes_pruned,
            cut_tree.n_leaves_cut(),
            cut_tree.fraction_leaves_covered()
        )]


def greedy_tour(edges: list[list[float]], timer: Timer) -> list[SolutionStats]:
    stats = []
    n = len(edges)
    n_nodes_expanded = 0
    best_cost = math.inf

    for i in range(n):
        start_city = i
        tour = [start_city]
        visited = set(tour)
        current_city = start_city
        
        while len(tour) <= n:
            if timer.time_out():
                break
            
            min_cost = math.inf
            next_city = -1
            for neighbor in range(n):
                if neighbor not in visited and edges[current_city][neighbor] < min_cost:
                    min_cost = edges[current_city][neighbor]
                    next_city = neighbor
                    
            if next_city == -1 or math.isinf(min_cost):
                break
            
            tour.append(next_city)
            visited.add(next_city)
            current_city = next_city
            n_nodes_expanded += 1
            
        if len(tour) == n:
            cost = score_tour(tour, edges)
            if not math.isinf(cost) and cost < best_cost:
                best_cost = cost
                stats = [SolutionStats(
                    tour=tour,
                    score=cost,
                    time=timer.time(),
                    max_queue_size=0,
                    n_nodes_expanded=n_nodes_expanded,
                    n_nodes_pruned=0,
                    n_leaves_covered=0,
                    fraction_leaves_covered=0.0
                )]
                
    return stats


def dfs(edges: list[list[float]], timer: Timer) -> list[SolutionStats]:
    stats = []
    n = len(edges)
    bssf = math.inf
    
    stack = [[0]]
    n_nodes_expanded = 0
    n_nodes_pruned = 0
    
    while stack:
        if timer.time_out():
            break
        
        path = stack.pop()
        n_nodes_expanded += 1
        
        if len(path) == n:
            path.append(path[0])
            cost = score_tour(path, edges)
            
            if cost < bssf:
                bssf = cost
                stats.append(SolutionStats(
                    tour=path[:-1],
                    score=cost,
                    time=timer.time(),
                    max_queue_size=len(stack),
                    n_nodes_expanded=n_nodes_expanded,
                    n_nodes_pruned=n_nodes_pruned,
                    n_leaves_covered=0,
                    fraction_leaves_covered=0.0
                ))
        else:
            last_city = path[-1]
            for next_city in range(n):
                if next_city not in path and not math.isinf(edges[last_city][next_city]):
                    stack.append(path + [next_city])
                    
    return stats
 
    
def branch_and_bound(edges: list[list[float]], timer: Timer) -> list[SolutionStats]:
    n = len(edges)
    stats = []
    n_nodes_expanded = 0
    n_nodes_pruned = 0
    cut_tree = CutTree(n)
    
    greedy_solution = greedy_tour(edges, timer)
    bssf = greedy_solution[-1].score
    
    pq = []
    path = [0]
    reduced_matrix, reduction_cost = reduce_matrix([row[:] for row in edges])
    heapq.heappush(pq, (reduction_cost, path, reduced_matrix, reduction_cost))
    
    max_queue_size = 0
    
    while pq:
        if timer.time_out():
            break
        
        current_cost, current_path, current_matrix, current_reduction_cost = heapq.heappop(pq)
        max_queue_size = max(max_queue_size, len(pq))
        n_nodes_expanded += 1
        
        if len(current_path) == n:
            tour_cost = current_cost
            if tour_cost < bssf:
                bssf = tour_cost
                stats.append(SolutionStats(
                    tour=current_path,
                    score=tour_cost,
                    time=timer.time(),
                    max_queue_size=max_queue_size,
                    n_nodes_expanded=n_nodes_expanded,
                    n_nodes_pruned=n_nodes_pruned,
                    n_leaves_covered=cut_tree.n_leaves_cut(),
                    fraction_leaves_covered=cut_tree.fraction_leaves_covered(),
                ))
            continue
        
        last_city = current_path[-1]
        for next_city in range(n):
            if next_city not in current_path and not math.isinf(current_matrix[last_city][next_city]):
                path_cost = current_matrix[last_city][next_city]
                new_matrix = [row[:] for row in current_matrix]
                
                for i in range(n):
                    new_matrix[last_city][i] = math.inf
                    new_matrix[i][next_city] = math.inf
                new_matrix[next_city][last_city] = math.inf
                
                reduced_matrix, reduction_cost = reduce_matrix(new_matrix)
                new_cost = current_reduction_cost + path_cost + reduction_cost
                
                if new_cost < bssf:
                    heapq.heappush(pq, (new_cost, current_path + [next_city], reduced_matrix, new_cost))
                else:
                    n_nodes_pruned += 1
                cut_tree.cut(current_path + [next_city])
                
    if not stats:
        stats = greedy_solution
    
    return stats


def branch_and_bound_smart(edges: list[list[float]], timer: Timer) -> list[SolutionStats]:
    n = len(edges)
    stats = []
    n_nodes_expanded = 0
    n_nodes_pruned = 0

    greedy_solution = greedy_tour(edges, timer)
    bssf = greedy_solution[-1].score

    pq = []
    path = [0]
    reduced_matrix, initial_reduction_cost = reduce_matrix([row[:] for row in edges])
    heapq.heappush(pq, (-len(path), initial_reduction_cost, path, reduced_matrix, initial_reduction_cost))
    
    max_queue_size = 0
    phase = 1
    phase_transition_time = timer.time()

    while pq:
        if timer.time_out():
            break

        _, current_cost, current_path, current_matrix, current_reduction_cost = heapq.heappop(pq)
        max_queue_size = max(max_queue_size, len(pq))
        n_nodes_expanded += 1

        elapsed_time = timer.time() - phase_transition_time
        if phase == 1 and len(stats) > 0 and elapsed_time > 5:
            phase = 2

        if len(current_path) == n:
            tour_cost = current_cost
            if tour_cost < bssf:
                bssf = tour_cost
                stats.append(SolutionStats(
                    tour=current_path,
                    score=tour_cost,
                    time=timer.time(),
                    max_queue_size=max_queue_size,
                    n_nodes_expanded=n_nodes_expanded,
                    n_nodes_pruned=n_nodes_pruned,
                    n_leaves_covered=0,
                    fraction_leaves_covered=0.0,
                ))
            continue

        last_city = current_path[-1]
        for next_city in sorted(range(n), key=lambda c: current_matrix[last_city][c]):
            if next_city not in current_path and not math.isinf(current_matrix[last_city][next_city]):
                path_cost = current_matrix[last_city][next_city]
                new_matrix = [row[:] for row in current_matrix]

                for i in range(n):
                    new_matrix[last_city][i] = math.inf
                    new_matrix[i][next_city] = math.inf
                new_matrix[next_city][last_city] = math.inf

                reduced_matrix, reduction_cost = reduce_matrix(new_matrix)
                new_cost = current_reduction_cost + path_cost + reduction_cost

                if new_cost < bssf:
                    priority = calculate_priority(phase, current_path, new_cost)
                    heapq.heappush(pq, (priority, new_cost, current_path + [next_city], reduced_matrix, new_cost))
                else:
                    n_nodes_pruned += 1

    if not stats:
        stats = greedy_solution

    return stats


def calculate_priority(phase, current_path, new_cost):
    path_length = len(current_path)
    if phase == 1:
        # Explore deeper first
        return (-path_length, new_cost)
    else:
        # Exploit lower costs
        return (new_cost, -path_length)


def reduce_matrix(matrix):
    n = len(matrix)
    reduction_cost = 0
    for i in range(n):
        min_value = min(matrix[i][j] for j in range(n) if j != i)
        if not math.isinf(min_value):
            reduction_cost += min_value
            for j in range(n):
                if i != j:
                    matrix[i][j] -= min_value
                
    for i in range(n):
        min_value = min(matrix[j][i] for j in range(n) if j !=i)
        if not math.isinf(min_value):
            reduction_cost += min_value
            for j in range(n):
                if i != j:
                    matrix[j][i] -= min_value
                
    return matrix, reduction_cost
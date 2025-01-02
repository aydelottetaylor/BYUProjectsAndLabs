# Uncomment this line to import some functions that can help
# you debug your algorithm
from plotting import draw_line, draw_hull, circle_point


from functools import cmp_to_key


mid = [0, 0]


def compute_hull(points: list[tuple[float, float]]) -> list[tuple[float, float]]:
    """Return the subset of provided points that define the convex hull"""
    sorted_points = sorted(points)


    convex_hull = divide_hull(sorted_points)


    return [(p[0], p[1]) for p in convex_hull]


def divide_hull(points):
    if len(points) <= 3:
        return brute_force_hull(points)
    
    mid = len(points) // 2
    left_hull = divide_hull(points[:mid])
    right_hull = divide_hull(points[mid:])


    return merge_hulls(left_hull, right_hull)


def brute_force_hull(points):
    global mid
    s = set()
    for i in range(len(points)):
        for j in range(i + 1, len(points)):
            x1, x2 = points[i][0], points[j][0]
            y1, y2 = points[i][1], points[j][1]


            a1, b1, c1 = y1 - y2, x2 - x1, x1 * y2 - y1 * x2
            pos, neg = 0, 0


            for k in range(len(points)):
                if k == i or k == j or a1 * points[k][0] + b1 * points[k][1] + c1 <= 0:
                    neg += 1
                if k == i or k == j or a1 * points[k][0] + b1 * points[k][1] + c1 >= 0:
                    pos += 1
            if pos == len(points) or neg == len(points):
                s.add(tuple(points[i]))
                s.add(tuple(points[j]))


    hull = [list(x) for x in s]
    mid = [sum(p[0] for p in hull) / len(hull), sum(p[1] for p in hull) / len(hull)]
    hull = sorted(hull, key=cmp_to_key(compare_points))


    return hull


def merge_hulls(left, right):
    n1, n2 = len(left), len(right)
    ileft, iright = 0, 0


    for i in range(1, n1):
        if left[i][0] > left[ileft][0]:
            ileft = i


    for i in range(1, n2):
        if right[i][0] < right[iright][0]:
            iright = i


    indleft, indright = ileft, iright
    done = False
    while not done:
        done = True
        while get_orientation(right[indright], left[indleft], left[(indleft + 1) % n1]) > 0:
            indleft = (indleft + 1) % n1
        while get_orientation(left[indleft], right[indright], right[(n2 + indright - 1) % n2]) <= 0:
            indright = (indright - 1) % n2
            done = False
    upperleft, upperright = indleft, indright


    indleft, indright = ileft, iright
    done = False
    while not done:
        done = True
        while get_orientation(left[indleft], right[indright], right[(indright + 1) % n2]) >= 0:
            indright = (indright + 1) % n2
        while get_orientation(right[indright], left[indleft], left[(n1 + indleft - 1) % n1]) <= 0:
            indleft = (indleft - 1) % n1
            done = False
    lowerleft, lowerright = indleft, indright


    merged_hulls = []


    ind = upperleft
    merged_hulls.append(left[upperleft])
    while ind != lowerleft:
        ind = (ind + 1) % n1
        merged_hulls.append(left[ind])
    
    ind = lowerright
    merged_hulls.append(right[lowerright])
    while ind != upperright:
        ind = (ind + 1) % n2
        merged_hulls.append(right[ind])


    return merged_hulls


def compare_points(point1, point2):
    point_uno = [point1[0] - mid[0], point1[1] - mid[1]]
    point_dos = [point2[0] - mid[0], point2[1] - mid[1]]


    one = quadrant_of_point(point_uno)
    two = quadrant_of_point(point_dos)


    if one != two:
        if one < two:
            return -1
        return 1
    if point_uno[1] * point_dos[0] < point_dos[1] * point_uno[0]:
        return -1
    return 1


def get_orientation(point1, point2, point3):
    res = (point2[1] - point1[1]) * (point3[0] - point2[0]) - (point3[1] - point2[1]) * (point2[0] - point1[0])
    if res == 0:
        return 0
    if res > 0:
        return 1
    return -1

def quadrant_of_point(point):
    if point[0] >= 0 and point[1] >= 0:
        return 1
    if point[0] <= 0 and point[1] >= 0:
        return 2
    if point[0] <= 0 and point[1] <= 0:
        return 3
    return 4
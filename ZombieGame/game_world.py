# layer 0: Background Objects
# layer 1: Foreground Objects
objects = [[], [], [], [] ,[]]

# collision information
# ex) key : 'boy:balls' string
# ex) value : [ [boy], [ball1, ball2, ball3] ]
collision_group = dict()

def add_object(o, depth):
    objects[depth].append(o)

def add_objects(ol, depth):
    objects[depth] += ol

def remove_object(o):
    for layer in objects:
        if o in layer:
            layer.remove(o)
            remove_collision_object(o) # 만들어둔 collision_group 에서도 객체 삭제
            del o
            return
    raise ValueError('Trying destroy non existing object')

def all_objects():
    for layer in objects:
        for o in layer:
            yield o


def clear():
    for o in all_objects():
        del o
    for layer in objects:
        layer.clear()
    collision_group.clear()

def add_collision_pairs(a,b,group):
    if group not in collision_group:
        print('Add new group ', group)
        collision_group[group] = [[], []] # list of list : list pair

    if a:
        if type(a) is list:
            collision_group[group][0] += a
        else:
            collision_group[group][0].append(a)

    if b:
        if type(b) is list: # 마안약 전달 값이 list 형태일 경우
            collision_group[group][1] += b
        else:
            collision_group[group][1].append(b)

def all_collision_pairs():
    for group, pairs in collision_group.items():
        for a in pairs[0]:
            for b in pairs[1]:
                yield a,b,group

def remove_collision_object(o): # collision_group에 저장된 오브젝트 삭제
    for pairs in collision_group.values(): # 키는 필요 X
        if o in pairs[0]: # 만약 o 가 첫번째 그룹(a)에 있으면
            pairs[0].remove(o) # a 그룹에서 제거
        elif o in pairs[1]: # 만약 o 가 두번째 그룹(b)에 있으면
            pairs[1].remove(o) # b 그룹에서 제거
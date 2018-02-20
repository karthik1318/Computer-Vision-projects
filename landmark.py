landmarks = [5, 12, 18, 25]
final_list = []
ps= []
for i in range(1,31):
    for j in range(len(landmarks)):
        if i in landmarks or i == landmarks[j]+1 or i == landmarks[j]-1:
            final_list.append(1)
            break
        else:
            continue
    final_list.append(0)
print final_list

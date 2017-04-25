
from __future__ import with_statement
from __future__ import division
from dtree import *
from collections import Counter
import sys
import os.path
import random
import numpy
sys.setrecursionlimit(100000)



myargs=sys.argv
ensemble=myargs[3]
bag_size=myargs[4]
depth=myargs[5]
depth=int(depth)
datapath=myargs[6]
def get_filenames():
    training_filename = sys.argv[1]
    test_filename = sys.argv[2]
    return training_filename,test_filename

def get_attributes(filename):
    with open(filename, 'r') as fin:
        header = fin.readline().strip()
    attributes = [attr.strip() for attr in header.split(",")]

    return attributes

def get_data(filename, attributes):
    with open(filename) as fin:
        lines = [line.strip() for line in fin.readlines()]
    del lines[0]
    data = []
    for line in lines:
        data.append(dict(zip(attributes,
                             [a.strip() for a in line.split(",")])))

    return data

def load_data(datapath):
    for file in os.listdir(datapath):
        if file.endswith("test1.csv"):
            test_filename = file
        elif file.endswith("train1.csv"):
            train_filename = file


def confusion_matrix(tn, tp, fn, fp):
    print ('      ',"real 1","real 0")
    print("-------------"*2)
    print ("exp 1 ",  str(tp) + '       ' + str(fp))
    print ("exp 0 ",  str(fn) + '       ' + str(tn))
    print("-----------------------"*2)


def bag_boost(h1,cc,ac,mc):
    initial_weights=[]
    for i in range(len(training_data)):
        initial_weights.append(0)
    for d,num in zip(training_data,initial_weights):
        d['weight']=num
    new_weight=[]
    for i in range(len(mc)):
        new_weight.append(0)
    for d,num in zip(mc,new_weight):
        d['weight']=num

    e=h1*1.0/len(training_data)*1.0
    ePositive=math.sqrt((1-e)/e)
    eNegetive=math.sqrt(e/1-e)
    normalize=2*(math.sqrt(e*(1-e)))
    for i in range(len(cc)):
        cc[i]['weight']=ePositive

    for i in range(len(mc)):
        mc[i]['weight']=eNegetive*normalize

    final_list=[]
    cc.extend(mc)
    for mydict in cc:
        if mydict not in mc:
            final_list.append(mydict)

    second_tree=ID3(final_list, attributes, target_attr, gain, depth,ensemble)
    second_classification = classify(second_tree,test_data)
    misclass1=[]
    mc=[]
    c=[]
    new_clas1=[]
    correct_classified1=[]
    for item in second_classification:
        new_clas1.append(item)
    boost_c11=0
    boost_c22=0
    boost_c33=0
    boost_c44=0
    for i in range(len(new_clas1)):
        if ac[i]=='1' and new_clas1[i]=='0':
            boost_c11+=1
            mc.append(test_data[i])
        elif ac[i]=='0' and new_clas1[i]=='1':
            misclass1.append(test_data[i])
            boost_c22+=1
        elif (ac[i]=='0'and new_clas1[i]=='0'):
            boost_c33+=1
            correct_classified1.append(test_data[i])
        elif (ac[i]=='1'and new_clas1[i]=='1'):
            boost_c44+=1
            c.append(test_data[i])
    misclass1=mc+misclass1
    correct_classified1=correct_classified1+c
    total_misclassification1=boost_c11+boost_c22
    #print(boost_c11)
    #print(boost_c22)
    #print(boost_c33)
    #print(boost_c44)
    return (total_misclassification1,misclass1,ac,correct_classified1,boost_c11,boost_c22,boost_c33,boost_c44)




def boosting(training_data,test_data,depth):
    #####new portion
    ac=[]
    for i in range(len(test_data)):
        ac.append(test_data[i]['bruises?-bruises'])
    dtree = ID3(training_data, attributes, target_attr, gain, depth,ensemble)
    classification = classify(dtree,test_data)
    new_clas=[]
    misclass=[]
    correct_classified=[]
    for item in classification:
        new_clas.append(item)
    boost_c1=0
    boost_c2=0
    for i in range(len(new_clas)):
        if ac[i]=='1' and new_clas[i]=='0':
            boost_c1+=1
        elif ac[i]=='0' and new_clas[i]=='1':
            misclass.append(test_data[i])
            boost_c2+=1
        else:
            correct_classified.append(test_data[i])

    total_miss=[]
    total_misclassification=boost_c1+boost_c2
    h1,cc,ac,mc,b1,b2,b3,b4=bag_boost(total_misclassification,correct_classified,ac,misclass)
    for i in range(int(bag_size)):
        bag_boost(h1,cc,ac,mc)
    fn=b1
    tp=b4
    fp=b2
    tn=b3
    fp=fp+tp
    confusion_matrix(tn,tp,fn,fp)
    accuracy=(tn)/len(new_clas)*100
    print(accuracy)


def bagging(training_data,test_data,bag_size,depth):
    avg=[]
    for i in range(int(bag_size)):
        actual=[]
        random.shuffle(test_data)
        random.shuffle(training_data)
        for i in range(len(test_data)):
            actual.append(test_data[i]['bruises?-bruises'])
        dtree = ID3(training_data, attributes, target_attr, gain, depth,ensemble)
        classification = classify(dtree,test_data)

        clas=[]
        for item in classification:
            clas.append(item)
        tn = 0
        tp = 0
        fn = 0
        fp = 0
        for i in range(len(clas)):
            if actual[i]=='0' and clas[i]=='0':
                #count+=1
                tn
                tn+=1
            elif actual[i]=='1' and clas[i]=='1':
                #np+=1
                tp
                tp+=1
            elif actual[i]=='1' and clas[i]=='0':
                #c1+=1
                fn
                fn+=1
            elif actual[i]=='0' and clas[i]=='1':
                #c2+=1
                fp
                fp+=1

        confusion=Counter(classification)
        # matrix=confusion_matrix(confusion,count,np,c1,c2)
        matrix=confusion_matrix(tn,tp,fn,fp)
        # accuracy=((count+np)/len(clas))*100
        accuracy=((tn+tp)/len(clas))*100
        avg.append(accuracy)
    print(sum(avg)/len(avg))

if __name__ == "__main__":
    load_data(datapath)
    training_filename, test_filename = get_filenames()
    attributes = get_attributes(training_filename)
    target_attr = attributes[20]
    print(target_attr)

    # Get the training and test data from the given files
    training_data = get_data(training_filename, attributes)
    test_data = get_data(test_filename, attributes)
    if ensemble=="bag":
        final=bagging(training_data,test_data,bag_size,depth)
    elif ensemble=="boost":
        final1=boosting(training_data,test_data,depth)



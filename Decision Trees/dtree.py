
"""
References : 
https://en.wikipedia.org/wiki/ID3_algorithm
https://github.com/croach/dtree
"""

from collections import *
import collections
import math

def check(data,target_attr):
    value={}
    for record in data:
        key = record[target_attr]
        if (value.has_key(key)):
            value[key] += 1.0
        else:
            value[key] = 1.0
    return value



def entropy(data, target_attr,ensemble):
    value = {}
    data_entropy = 0.0
    # Calculate the frequency of each of the values in the target attr
    p=check(data,target_attr)
    for f in p.values():
        data_entropy += (-f/len(data)) * math.log(f/len(data), 2)

    return data_entropy

def gain(data, attr, target_attr, ensemble):
    val_freq = {}
    subset_entropy = 0.0
    for record in data:
        key = record[attr]
        if (val_freq.has_key(key)):
            val_freq[key] += 1.0
        else:
            val_freq[key] = 1.0

    for val in val_freq.keys():
        val_sum = sum(val_freq.values())
        val_prob = val_freq[val] / val_sum
        data_subset = []
        for record in data:
            if record[attr] == val:
                data_subset.append(record)

        subset_entropy += val_prob * entropy(data_subset, target_attr, ensemble)


    return (entropy(data, target_attr,ensemble) - subset_entropy)

def majority_value(data, target_attr):
    data = data[:]
    lst = []
    highest_freq = 0
    most_freq = None
    for record in data:
        lst.append(record[target_attr])
    
    """
    Returns a list made up of the unique values found in lst.
    """

    unique_lst = []
    for item in lst:
        if item not in unique_lst:
            unique_lst.append(item)  

    for val in unique_lst:
        if lst.count(val) > highest_freq:
            most_freq = val
            highest_freq = lst.count(val)
    
    return most_freq

def get_values(data, attr):
    data = data[:]
    lst = []
    for record in data:
        lst.append(record[attr])
    
    """
    Returns a list made up of the unique values found in lst.
    """
    
    unique_lst = []
    for item in lst:
        if item not in unique_lst:
            unique_lst.append(item)  

    return unique_lst

def choose_attribute(data, attributes, target_attr, fitness, ensemble):
    data = data[:]
    best_gain = 0.0
    best_attr = None

    for attr in attributes:
        gain = fitness(data, attr, target_attr, ensemble)
        if (gain >= best_gain and attr != target_attr):
            best_gain = gain
            best_attr = attr

    return best_attr

def get_examples(data, attr, value):
    data = data[:]
    rtn_lst = []

    if not data:
        return rtn_lst
    else:
        record = data.pop()
        if record[attr] == value:
            rtn_lst.append(record)
            rtn_lst.extend(get_examples(data, attr, value))
            return rtn_lst
        else:
            rtn_lst.extend(get_examples(data, attr, value))
            return rtn_lst

def get_classification(record, tree):

    if type(tree) == type("string"):
        return tree
    else:
        attr = tree.keys()[0]
        t = tree[attr][record[attr]]
        return get_classification(record, t)

def classify(tree, data):
    data = data[:]
    classification = []

    for record in data:
        classification.append(get_classification(record, tree))
    return classification

def ID3(data, attributes, target_attr, fitness_func, depth, ensemble):
    data = data[:]
    vals = [record[target_attr] for record in data]
    default = majority_value(data, target_attr)
    if not data or (len(attributes) - 1) <= 0 or depth == 0:
        return default
    elif vals.count(vals[0]) == len(vals):
        return vals[0]
    else:
        depth -= 1
        best = choose_attribute(data, attributes, target_attr,
                                fitness_func, ensemble)
        tree = {best:collections.defaultdict(lambda: default)}
        for val in get_values(data, best):
            sub_examples = get_examples(data, best, val)
            sub_attributes = []
            for attr in attributes:
                if attr != best:
                    sub_attributes.append(attr)
            subtree = ID3(
                sub_examples,
                sub_attributes,
                target_attr,
                fitness_func, depth, ensemble)
            tree[best][val] = subtree

    return tree
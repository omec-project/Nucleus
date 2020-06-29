# 
# Copyright 2019-present Infosys Limited
# 
# SPDX-License-Identifier: Apache-2.0
# 

import os
from os import listdir
from os.path import isfile, join

# Globals
ttFileName = ''
includeset = ''
outputDir = ''
outputFile = ''
outputFileKeyword = ''
outputFileExt = ''
outputFileName = ''
mode = ''

def WriteFile(dirName, fileName, content, fileMode):
    filePath = join(dirName, fileName)
    accessMode = 'w+'
    if (fileMode == 'append') :
        accessMode = 'a+'
    with open(filePath, accessMode) as File:
        File.write(content)
    
def readFile(fileName):
    if os.path.exists(fileName):
        with open(fileName, 'r') as file:
            return file.read()
    return

def searchWordInDir(path, word):
    files = [f for f in listdir(path) if isfile(join(path, f))]
    for i in files:
        fileContent = open(join(path, i)).read()
        if word in fileContent:
            return True
    return False


def searchWordInFile(path, fileName, word):
    if os.path.exists(join(path, fileName)):
        with open(join(path, fileName)) as File:
            fileContent = File.read()
            if word in fileContent:
                return True
    return False

def isFileEmpty(path, fileName):
    fullPath = join(path, fileName)
    if os.path.exists(fullPath):
        if os.stat(fullPath).st_size > 0:
            return 0
        else :
            return 1
    return 1

def getFileName(str):
    if (str.find("_") != -1):
        str = str.lower()
        tokens = [x.capitalize() for x in str.split("_")]
        fileName = ''
        fileName = fileName.join(tokens)
        return (fileName[0].lower() + fileName[1:])

    return (str[0].lower() + str[1:])

def get_key_values(obj, key):
    arr = []
    
    def extract(obj, arr, key):
        if isinstance(obj, dict):
            for k,v in obj.items():
                if k == key:
                    arr.append(v)
                elif isinstance(v, (dict, list)):
                    extract(v, arr, key)
        elif isinstance(obj,list):
            for item in obj:
                extract(item, arr, key)       
        return arr
    
    results = extract (obj, arr, key)
    return results

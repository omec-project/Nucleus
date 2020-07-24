# 
# Copyright 2019-present Infosys Limited
# 
# SPDX-License-Identifier: Apache-2.0
# 

import json, sys, os
from os.path import join
import utils
#set path where you have Template-Toolkit 
#sys.path.append("<path>/Template-Toolkit-Python")

def processTemplate(templateIp, appModelJSON):
    from template import Template
    
    utils.outputFileName = ''
    if utils.outputFileKeyword != '':
        utils.outputFileName = utils.getFileName(templateIp[utils.outputFileKeyword])    
    if utils.outputFile != '':
        utils.outputFileName = utils.outputFileName + utils.outputFile
    
    utils.outputFileName = utils.outputFileName + utils.outputFileExt

    tt = Template({'EVAL_PYTHON':1, 'AUTO_RESET':1})
    op = tt.process(utils.ttFileName,
                    {
                        'TemplateInputVar' : templateIp,
                        'includes' : utils.includeSet,
                        'AppModelJSON' : appModelJSON
                    })
    utils.WriteFile(utils.outputDir, utils.outputFileName, op, utils.mode)

def getTemplateIn(appModelItems, depth, appModelJSON):
    depthChanged = False

    if (depth == 0):
        print(appModelItems)
        processTemplate(appModelItems, appModelJSON)
        return
    
    if depthChanged == False:
        depth = depth - 1
        depthChanged = True
        
    for item in appModelItems:
        getTemplateIn(item, depth, appModelJSON)
      
def genCppCode(genModel, appModelJSON):
    for item in genModel:
        keyWord = item["appModelKeyword"]
        depth = item["appModelValueDepth"]
        if 'includes' in item:
            utils.includeSet = item["includes"]
        else :
            utils.includeSet = ""
        utils.ttFileName = item["templateFile"]
        utils.outputDir = item["outputPath"]
        utils.outputFile = item["outputFile"]
        utils.outputFileKeyword = item["outputFileKey"]
        utils.outputFileExt = item["outputFileExt"]
        utils.mode = item["mode"]
        
        if not os.path.exists(utils.outputDir):
            os.makedirs(utils.outputDir)
        
        appModelItems = utils.get_key_values(appModelJSON, keyWord) 
        getTemplateIn(appModelItems, depth, appModelJSON)
            
with open('dataModels/generationItem.json') as JSONFile:
    GenItemJSON = json.load(JSONFile)
    for model in GenItemJSON['Models']:
        with open(model['AppModel']) as jsonFile:
            appModelJson = json.load(jsonFile)
            genCppCode(model['genCode'], appModelJson)

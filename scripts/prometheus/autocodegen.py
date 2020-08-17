#Pending Points
#Get port number  as argument 
#IP address looks optional 
import json

gauges_family_object_list = [] #type class Gauge
counter_family_object_list = [] #tupe class Counter
module_name = "mmeStats"


#define class 
class Gauge:
    def __init__(self, name, common_labels, specific_label):
        self.familyname = name
        print("common_labels = {} and specific_label = {} ".format(common_labels, specific_label))
        #generate enum name 
        e_name = name
        for l in common_labels:
           print("l = {}".format(l)) 
           for k in l.keys():
              v = l[k]
              e_name = e_name + "_" + k + "_" + v
        for l in specific_label:
           print("l = {}".format(l)) 
           for k in l.keys():
              v = l[k]
              e_name = e_name + "_" + k + "_" + v
        self.enum_name = e_name.upper()
        print("enum name {}".format(self.enum_name))

        labels = "{"
        for l in common_labels:
            if labels != "{":
              labels = labels + ","
            for k in l.keys():
               v = l[k]
               labels = labels + "{\"" + k + "\",\"" + v + "\"}"
        for l in specific_label:
           if labels != "{":
             labels = labels + ","
           for k in l:
              v = l[k]
              labels = labels + "{\"" + k + "\",\"" + v + "\"}"
        labels = labels + "}" 
        self.gauge_labels = labels
        print("Gauge labels = {}".format(self.gauge_labels))
            
        labelk = ""
        labelv = ""
        labeldict = common_labels
        for l in specific_label:
           for k in l.keys():
              v = l[k]
              labeldict.append({k:v})
              labelk = k
              labelv = v

        self.labeldict = labeldict
        print("Gauge labeldict = {}".format(labeldict))
 
        self.gauge_name = "current_" + labelk + "_" + labelv

        print("\n\tGauge \n\t\tFamily {} \n\t\tgauge name - {} \n\t\tlabels {}\n".format(self.familyname, self.gauge_name, self.gauge_labels))

class GaugeFamily:
    def __init__(self, family, familyname, familyhelp, labeldict):
        self.family = family
        self.familyname = familyname
        self.familyhelp = familyhelp
        #self.familylabelk = labelk
        #self.familylabelv = labelv
        print("Gaugefamily labeldict = {}".format(labeldict))
        labels = "{"
        for l in labeldict:
           if labels != "{":
             labels = labels + ","
           for k in l:
              v = l[k]
              labels = labels + "{\"" + k + "\",\"" + v + "\"}"
              labelk = k
              labelv = v
        labels = labels + "}" 

        print("Gaugefamily labels = {}".format(labels))
        self.family_labels = labels
        self.classname = family + "_gauges"
        self.promfamily = family + "_family"
        self.moduleStatsMember = family + "_m"
        self.gaugeMetricList = []
        print("Gaugefamily :  \n\tFamily - {} \n\t\tFamilyName - {} \n\t\tFamilyHelp {} \n\t\tFamilyLabels {} \n\t\tClassname {} \n\t\tPromFamily = {}****** \n".format(self.family, self.familyname, self.familyhelp, self.family_labels, self.classname, self.promfamily))

    def add_gauge(self, family, common_label, specific_label): 
        g = Gauge(family, common_label, specific_label)
        self.gaugeMetricList.append(g)

class Counter:
    def __init__(self, family, metric_name, common_labels, specific_label):
        self.family = family
        self.metric_name = metric_name 
        #self.labeldict = labeldict
        for l in specific_label:
           for v in l.values():
              self.counter_name = metric_name + "_" + v

        e_name = family

        print("Counter common_labels = {}".format(common_labels))
        print("Counter specific_label = {}".format(specific_label))
        for l in common_labels:
           for v in l.values():
              e_name = e_name + "_" + v
        for l in specific_label:
           for v in l.values():
              e_name = e_name + "_" + v
        self.enum_name = e_name.upper()
        print("enum name {}".format(self.enum_name))

        labels = "{"
        for l in common_labels:
           if labels != "{":
             labels = labels + ","
           for k in l.keys():
              v = l[k]
              labels = labels + "{\"" + k + "\",\"" + v + "\"}"
        for l in specific_label:
           if labels != "{":
             labels = labels + ","
           for k in l.keys():
              v = l[k]
              labels = labels + "{\"" + k + "\",\"" + v + "\"}"
        labels = labels + "}" 
        self.counter_labels = labels

        labeldict = common_labels
        for l in specific_label:
           for k in l.keys():
              v = l[k]
              labeldict.append({k:v})

        self.labeldict = labeldict
        print("final labeldict = {}".format(labeldict))
        print("\n\tCounter \n\t\tFamily {} \n\t\tCounterName {} \n\t\tlabels {}\n".format(self.family, self.counter_name, self.counter_labels))

class CounterFamily:
    def __init__(self, family, familyname, familyhelp, labeldict):
        self.family = family
        self.familyname = familyname
        self.familyhelp = familyhelp
        self.classname = family + "_counters"
        self.promfamily = family + "_family"
        self.moduleStatsMember = family + "_m"
        print("CounterFamily labeldict = {}".format(labeldict))
        labels = "{"
        for l in labeldict:
           if labels != "{":
             labels = labels + ","
           print("l = {}".format(l)) 
           for k in l:
              v = l[k]
              labels = labels + "{\"" + k + "\",\"" + v + "\"}"
        labels = labels + "}" 
        self.family_labels = labels
        print("\n**** CounterFamily :  \n\tFamily {} \n\tFamilyname {} \n\tFamilyhelpStr {} \n\tFamilylabels {} \n\tClassname - {} \n\tPromFamily {}******\n".format(self.family, self.familyname, self.familyhelp, self.family_labels, self.classname, self.promfamily))
        self.counterMetricList = []

    def add_counter(self, family, metric_name, common_labels, specific_label): 
        c = Counter(family, metric_name, common_labels, specific_label)
        self.counterMetricList.append(c)

def add_copyright(fh):
    fh.write("/*\n")
    fh.write(" * Copyright 2020-present Open Networking Foundation\n")
    fh.write(" *\n")
    fh.write(" * SPDX-License-Identifier: Apache-2.0\n")
    fh.write(" */\n")

def add_header_macro(fh):
    h = "#ifndef _INCLUDE_"+module_name+"_H__\n"
    fh.write(h)
    h = "#define _INCLUDE_"+module_name+"_H__\n"
    fh.write(h)

def add_prom_headers(fh):
    fh.write("\n") 
    fh.write("#include <prometheus/gauge.h>\n")
    fh.write("#include <prometheus/counter.h>\n")
    fh.write("#include <prometheus/exposer.h>\n")
    fh.write("#include <prometheus/registry.h>\n")
    fh.write("\n") 

def add_namespace_variables(fh):
    fh.write("\n") 
    fh.write("using namespace prometheus;\n")
    fh.write("extern std::shared_ptr<Registry> registry;\n")
    fh.write("\n") 

def add_setup_function(fh):
    fh.write("\n") 
    fname = module_name + "SetupPrometheusThread"
    declaration = "void " + fname + "(void);"
    fh.write(declaration)
    fh.write("\n") 

def add_counter_enum(fh):
    fh.write("\n") 
    fh.write("enum class " + module_name + "Counter {\n")
    enum_class = []
    for gauge in gauges_family_object_list:
      for metric in gauge.gaugeMetricList:
        enum_class.append(metric.enum_name)
    for counter in counter_family_object_list:
      for metric in counter.counterMetricList:
        enum_class.append(metric.enum_name)
    x = len(enum_class)
    for i in range(x-1):
        fh.write("\t" + enum_class[i].upper() + ",\n")
    fh.write("\t" + enum_class[x-1].upper() + "\n")
    fh.write("};\n") 
    fh.write("\n") 
   
def add_dynamic_node_hash_functions(fh):
    fh.write("struct Node \n"
    "{\n"
    "    mmeStatsCounter id;\n"
    "	std::string label_k;\n"
    "	std::string label_v;\n"
    "\n"
    "	Node(mmeStatsCounter id, std::string label_k, std::string label_v)\n"
    "	{\n"
    "		this->id = id;\n"
    "		this->label_k = label_k;\n"
    "		this->label_v = label_v;\n"
    "	}\n"
    "\n"
    "	// operator== is required to compare keys in case of hash collision\n"
    "	bool operator==(const Node &p) const\n"
    "	{\n"
    "		return label_k == p.label_k && label_v == p.label_v && id == p.id;\n"
    "	}\n"
    "};\n"
    "\n"
    "struct hash_fn\n"
    "{\n"
    "	std::size_t operator() (const Node &node) const\n"
    "	{\n"
    "		std::size_t h1 = std::hash<std::string>()(node.label_k);\n"
    "		std::size_t h2 = std::hash<std::string>()(node.label_v);\n"
    "		std::size_t h3 = std::size_t(node.id);\n"
    "		return h1 ^ h2 ^ h3;\n"
    "	}\n"
    "};\n")


def add_footer_macro(fh):
    fh.write("\n") 
    h = "#endif /* _INCLUDE_"+module_name+"_H__ */\n"
    fh.write(h)

def parse_json_file():
  with open('mmeStats.json') as f:
      data = json.load(f)
  families = data.keys()
  for key in families:
      family = data[key]
      family_name = key 
      print("*********family {} Type - {} *********".format(family_name, family['type']))
      nameStr = family["name"] 
      helpStr = family["help"] 
      print("family_labels {}".format(family['family_labels']))
      flabel_dict = family['family_labels']
     
      if family['type'] == "Gauge":
        gaugeFamilyObj = GaugeFamily(family_name, nameStr, helpStr, flabel_dict) 
        gauges_family_object_list.append(gaugeFamilyObj)
        metrics = family['gauges']
        for metric in metrics:
          labeldict = metric["static_label"]
          for onelabel in labeldict:
             onelabeldict = []
             for tempk in onelabel.keys():
                onelabeldict = [{tempk: onelabel[tempk]}]
             common_label_list = []
             if metric.get("common_label"):
               common_label_list = metric["common_label"]
               print("common_label_list = {}".format(common_label_list))
             print("specific label = {}".format(onelabeldict))
             gaugeFamilyObj.add_gauge(family_name, common_label_list, onelabeldict)
      else:
        counterFamilyObj = CounterFamily(family_name, nameStr, helpStr, flabel_dict) 
        counter_family_object_list.append(counterFamilyObj)
        metrics = family['counters']
        for metric in metrics:
          print("\n\n METRIC LOOP \n\n")
          static_labels = metric["static_label"]
          for static_label in static_labels:
             onelabeldict = []
             common_label_list = []
             print("\n** static_label = {}, common_label_list = {} ".format(static_label, common_label_list))
             for tempk in static_label.keys():
                static_label_dict = [{tempk: static_label[tempk]}]

             if metric.get("common_label"):
               print("**? common_label_list = {}".format(common_label_list))
               print("static_labels {}".format(static_labels))
               print("metric {}".format(metric))
               import copy
               common_label_list = copy.deepcopy(metric["common_label"])
               print("**x common_label_list = {}".format(common_label_list))

             print("**common_label_list = {}".format(common_label_list))
             print("**specific label = {}".format(static_label))
             counterFamilyObj.add_counter(family_name, metric['name'], common_label_list, static_label_dict)

def add_gauge_classes(fh):
    fh.write("\n\n")
    fh.write("class DynamicMetricObject" + " {\n")
    fh.write("\tpublic:\n")
    fh.write("};\n")

    for gauge in gauges_family_object_list:
      fh.write("\n\n")
      fh.write("class " + gauge.family + "_DynamicMetricObject : public DynamicMetricObject" + " {\n")
      fh.write("\tpublic:\n")
      dyn_fun_signature = {}
      for metric in gauge.gaugeMetricList:
        labels = len(metric.labeldict)
        if(dyn_fun_signature.get(labels)):
          continue
        constructor_args=""
        gauge_create_labels = "{"
        dynamic_function_call_args=""
        for index in range(labels):
          constructor_args += "std::string labelk"+ str(index) + ", std::string labelv"+ str(index) + ","
          gauge_create_labels += "{" + "labelk"+ str(index) +", labelv" + str(index) + "}"+","
          dynamic_function_call_args += "labelk" + str(index)+", labelv"+str(index) + "," 
        constructor_args += "std::string labelk, std::string labelv"
        gauge_create_labels += "{labelk, labelv}"
        dynamic_function_call_args += "labelk, labelv"
        gauge_create_labels += "}"
        dyn_fun_signature[labels] = [constructor_args, gauge_create_labels, dynamic_function_call_args]

      for f in dyn_fun_signature.values():
        constructor_args =f[0]
        gauge_create_labels = f[1]
        dynamic_function_call_args = f[2]
        fh.write("\t\t" + gauge.family + "_DynamicMetricObject(Family<Gauge> &" + gauge.promfamily + "," + constructor_args +"):\n")
        fh.write("\t\t DynamicMetricObject(),\n")
        fh.write("\t\t gauge(" + gauge.promfamily + ".Add(" + gauge_create_labels + "))\n") 
        fh.write("\t\t{\n")
        fh.write("\t\t}\n")

      fh.write("\t\t~" + gauge.family + "_DynamicMetricObject" + "()\n")
      fh.write("\t\t{\n")
      fh.write("\t\t}\n")
      fh.write("\t\tGauge &gauge;\n")
      fh.write("};\n")


      fh.write("class " + gauge.classname + " {\n")
      fh.write("\tpublic:\n")
      fh.write("\t" + gauge.classname + "();\n")
      fh.write("\t~" + gauge.classname + "();\n")
      fh.write("\tFamily<Gauge> &"+ gauge.promfamily + ";\n")
      for metric in gauge.gaugeMetricList:
        fh.write("\tGauge &" + metric.gauge_name + ";\n")

      for f in dyn_fun_signature.values():
        constructor_args,gauge_create_labels,dynamic_function_call_args = f
        fh.write("\n\t" + gauge.family+"_"+"DynamicMetricObject* add_dynamic(" + constructor_args + ") {\n")
        fh.write("\t\treturn new "+ gauge.family+"_"+"DynamicMetricObject("+ gauge.promfamily + "," + dynamic_function_call_args + ");\n ")
        fh.write("\t}\n")

      fh.write("};\n\n\n")

def add_counter_classes(fh):
    for counter in counter_family_object_list:
      fh.write("\n\n")
      fh.write("class " + counter.family + "_DynamicMetricObject : public DynamicMetricObject" + " {\n")
      fh.write("\tpublic:\n")
      dyn_fun_signature = {}
      for metric in counter.counterMetricList:
        labels = len(metric.labeldict)
        if(dyn_fun_signature.get(labels)):
          continue
        constructor_args=""
        counter_create_labels = "{"
        dynamic_function_call_args=""
        for index in range(labels):
          constructor_args += "std::string labelk"+ str(index) + ",std::string labelv"+ str(index) + ","
          counter_create_labels += "{" + "labelk"+ str(index) +", labelv" + str(index) + "}"+","
          dynamic_function_call_args += "labelk" + str(index)+",labelv"+str(index) + "," 
        constructor_args += "std::string labelk, std::string labelv"
        counter_create_labels += "{labelk, labelv}"
        dynamic_function_call_args += "labelk, labelv"
        counter_create_labels += "}"
        dyn_fun_signature[labels] = [constructor_args, counter_create_labels, dynamic_function_call_args]


      for f in dyn_fun_signature.values():
        constructor_args = f[0]
        counter_create_labels = f[1]
        dynamic_function_call_args = f[2]
        fh.write("\t\t" + counter.family + "_DynamicMetricObject(Family<Counter> &" + counter.promfamily + "," + constructor_args +"):\n")
        fh.write("\t\t DynamicMetricObject(),\n")
        fh.write("\t\t counter(" + counter.promfamily + ".Add(" + counter_create_labels + "))\n") 
        fh.write("\t\t{\n")
        fh.write("\t\t}\n")

      fh.write("\t\t~" + counter.family + "_DynamicMetricObject" + "()\n")
      fh.write("\t\t{\n")
      fh.write("\t\t}\n")
      fh.write("\t\tCounter &counter;\n")
      fh.write("};\n")


      fh.write("class " + counter.classname + " {\n")
      fh.write("\tpublic:\n")
      fh.write("\t" + counter.classname + "();\n")
      fh.write("\t~" + counter.classname + "();\n")
      fh.write("\tFamily<Counter> &"+ counter.promfamily + ";\n")
      for metric in counter.counterMetricList:
        fh.write("\tCounter &" + metric.counter_name + ";\n")


      for f in dyn_fun_signature.values():
        constructor_args,counter_create_labels,dynamic_function_call_args = f
        fh.write("\n\t" + counter.family+"_"+"DynamicMetricObject* add_dynamic(" + constructor_args + ") {\n")
        fh.write("\t\treturn new "+ counter.family+"_"+"DynamicMetricObject("+ counter.promfamily + "," + dynamic_function_call_args + ");\n ")
        fh.write("\t}\n")

      fh.write("};\n")
      fh.write("\n")

def add_module_stats_class_declaration(fh):
    fh.write("class " + module_name + " {\n")
    fh.write("\t public:\n")
    fh.write("\t\t" + module_name + "();\n")
    fh.write("\t\t~" + module_name + "() {}\n")
    fh.write("\t\tstatic " + module_name + "* Instance(); \n")
    fh.write("\t\tvoid " + module_name + "promThreadSetup(void);\n")
    fh.write("\t\tvoid increment("+module_name+"Counter name, std::map<std::string, std::string> labels={}); \n")
    fh.write("\t\tvoid decrement("+module_name+"Counter name, std::map<std::string, std::string> labels={}); \n")
    fh.write("\t public:\n")
    for gauge in gauges_family_object_list:
      fh.write("\t\t" + gauge.classname + " *" + gauge.moduleStatsMember + ";\n")
    for counter in counter_family_object_list:
      fh.write("\t\t" + counter.classname + " *" + counter.moduleStatsMember +";\n")

    fh.write("\t\tstd::unordered_map<struct Node, DynamicMetricObject*, hash_fn> metrics_map;\n")
    fh.write("};\n")

def create_header_file():
    fname = module_name + "PromClient.h"
    header_file = open(fname, 'w')
    add_copyright(header_file)
    add_header_macro(header_file)
    add_prom_headers(header_file)
    add_namespace_variables(header_file)
    add_setup_function(header_file)
    add_counter_enum(header_file)
    add_dynamic_node_hash_functions(header_file);
    add_gauge_classes(header_file)
    add_counter_classes(header_file)
    add_module_stats_class_declaration(header_file)
    add_footer_macro(header_file)

def add_stl_header(fh):
    fh.write("#include <iostream>\n")
    fh.write("#include <chrono>\n")
    fh.write("#include <map>\n")
    fh.write("#include <memory>\n")
    fh.write("#include <thread>\n")
    fh.write("#include \"" + module_name + "PromClient.h\"\n") 
    fh.write("\n") 
    fh.write("using namespace prometheus;\n")
    fh.write("std::shared_ptr<Registry> registry;\n")
    fh.write("\n") 

def add_prom_client_setup_function(fh):
    fname = module_name + "SetupPrometheusThread"
    fh.write("void " + fname + "(void)\n")
    fh.write("{\n")
    fh.write("    registry = std::make_shared<Registry>();\n")
    fh.write("    /* Create single instance */ \n")
    fh.write("    mmeStats::Instance(); \n")
    fh.write("    Exposer exposer{\"0.0.0.0:3081\", 1};\n")
    fh.write("    std::string metrics(\"/metrics\");\n")
    fh.write("    exposer.RegisterCollectable(registry, metrics);\n")
    fh.write("    while(1)\n")
    fh.write("    {\n")
    fh.write("        std::this_thread::sleep_for(std::chrono::seconds(1));\n")
    fh.write("    }\n")
    fh.write("}\n")

def add_module_stats_class_defination(fh):
    fh.write(module_name + "::" + module_name + "()\n")
    fh.write("{\n")
    for gauge in gauges_family_object_list:
      fh.write("\t " + gauge.moduleStatsMember  + " = new " + gauge.classname + ";\n")
    for counter in counter_family_object_list:
      fh.write("\t " + counter.moduleStatsMember + " = new " + counter.classname + ";\n")
  
    fh.write("}\n")

def add_singleton_obect_create(fh):
    fh.write(module_name + "* " + module_name + "::" + "Instance() \n")
    fh.write("{\n")
    fh.write("\tstatic " + module_name + " object;\n")
    fh.write("\treturn &object; \n")
    fh.write("}\n")
    
def add_gauge_class_defination(fh):
    for gauge in gauges_family_object_list:
      fh.write("\n\n")
      fh.write(gauge.classname + "::" + gauge.classname + "():\n")
      print("family labels {}".format(gauge.family_labels))
      fh.write(gauge.promfamily + "(BuildGauge().Name(\""+ gauge.familyname +"\").Help(\"" + gauge.familyhelp + "\").Labels(" + gauge.family_labels + ").Register(*registry))," + "\n")
      metrics_len = len(gauge.gaugeMetricList)
      for metric_index in range(metrics_len - 1):
        metric = gauge.gaugeMetricList[metric_index]
        fh.write(metric.gauge_name + "(" + gauge.promfamily + ".Add(" + metric.gauge_labels + ")),\n")

      metric = gauge.gaugeMetricList[metrics_len -1]
      fh.write(metric.gauge_name + "(" + gauge.promfamily + ".Add(" + metric.gauge_labels + "))\n")
      
      fh.write("{\n")
      fh.write("}\n")
      fh.write("\n\n")
      fh.write(gauge.classname + "::~" + gauge.classname + "()\n")
      fh.write("{\n")
      fh.write("}\n")
      fh.write("\n\n")

def add_counter_class_defination(fh):
    for counter in counter_family_object_list:
      print("family labels {}".format(counter.family_labels))
      fh.write("\n\n")
      fh.write(counter.classname + "::" + counter.classname + "():\n")
      fh.write(counter.promfamily + "(BuildCounter().Name(\""+ counter.familyname +"\").Help(\"" + counter.familyhelp + "\").Labels(" + counter.family_labels + ").Register(*registry))," + "\n")
      metrics_len = len(counter.counterMetricList)
      metric_final_label = ".Add({"
      found = False
      for metric_index in range(metrics_len - 1):
        metric = counter.counterMetricList[metric_index]
        fh.write(metric.counter_name + "(" + counter.promfamily + ".Add(" + metric.counter_labels + ")),\n")

      if(metrics_len>=1):
        metric_final_label = ".Add({"
        metric = counter.counterMetricList[metrics_len - 1 ]
        fh.write(metric.counter_name + "(" + counter.promfamily + ".Add(" + metric.counter_labels + "))\n")
      
      fh.write("{\n")
      fh.write("}\n")
      fh.write("\n\n")
      fh.write(counter.classname + "::~" + counter.classname + "()\n")
      fh.write("{\n")
      fh.write("}\n")
      fh.write("\n\n")

def add_increment_api(fh):
    fh.write("\n\n")
    fh.write("void " + module_name + "::" + "increment("+module_name+"Counter name,std::map<std::string,std::string> labels)\n") 
    fh.write("{\n")
    fh.write("\tswitch(name) {\n")
    for gauge in gauges_family_object_list:
      for metric in gauge.gaugeMetricList:
        labels = ""
        index = 0
        for l in metric.labeldict:
          if index != 0:
            labels += ","
          for k in l:
             v = l[k]
             labels += "\"" + k + "\"" + ",\"" + v + "\"" 
          index += 1

        fh.write("\tcase "+module_name+"Counter::" + metric.enum_name + ":\n")
        fh.write("\t{\n")
        fh.write("\t\t" + gauge.moduleStatsMember + "->" + metric.gauge_name + ".Increment();\n")
        fh.write("\t\tfor(auto it = labels.begin(); it != labels.end(); it++) {\n")
        fh.write("\t\tstd::cout<<\"label - (\"<<it->first<<\",\"<<it->second<<\")\"<<std::endl;\n")
        fh.write("\t\tstruct Node s1 = {name, it->first, it->second};\n")
        fh.write("\t\tauto it1 = metrics_map.find(s1);\n")
        fh.write("\t\tif(it1 != metrics_map.end()) {\n")
        fh.write("\t\t    "+gauge.family +"_DynamicMetricObject *obj = static_cast<"+gauge.family+"_DynamicMetricObject *>(it1->second);\n")
        fh.write("\t\t    obj->gauge.Increment();\n")
        fh.write("\t\t} else {\n")
        fh.write("\t\t    "+gauge.family+"_DynamicMetricObject *obj = " + gauge.moduleStatsMember + "->add_dynamic(" + labels + ",it->first, it->second);\n")
        fh.write("\t\t    auto p1 = std::make_pair(s1, obj);\n")
        fh.write("\t\t    metrics_map.insert(p1);\n")
        fh.write("\t\t    obj->gauge.Increment();\n")
        fh.write("\t\t}\n")
        fh.write("\t\t}\n")
        fh.write("\t\tbreak;\n")
        fh.write("\t}\n")

    for counter in counter_family_object_list:
      for metric in counter.counterMetricList:
        labels = ""
        index = 0
        for l in metric.labeldict:
          if index != 0:
            labels += ","
          for k in l:
             v = l[k]
             labels += "\"" + k + "\"" + ",\"" + v + "\"" 
          index += 1

        fh.write("\tcase "+module_name+"Counter::" + metric.enum_name + ":\n")
        fh.write("\t{\n")
        fh.write("\t\t" + counter.moduleStatsMember+ "->" + metric.counter_name + ".Increment();\n")
        fh.write("\t\tfor(auto it = labels.begin(); it != labels.end(); it++) {\n")
        fh.write("\t\tstd::cout<<\"label - (\"<<it->first<<\",\"<<it->second<<\")\"<<std::endl;\n")
        fh.write("\t\tstruct Node s1 = {name, it->first, it->second};\n")
        fh.write("\t\tauto it1 = metrics_map.find(s1);\n")
        fh.write("\t\tif(it1 != metrics_map.end()) {\n")
        fh.write("\t\t    "+ counter.family+ "_DynamicMetricObject *obj = static_cast<" + counter.family+ "_DynamicMetricObject *>(it1->second);\n")
        fh.write("\t\t    obj->counter.Increment();\n")
        fh.write("\t\t} else {\n")
        fh.write("\t\t    "+counter.family + "_DynamicMetricObject *obj = " + counter.moduleStatsMember + "->add_dynamic(" + labels + ",it->first, it->second);\n")
        fh.write("\t\t    auto p1 = std::make_pair(s1, obj);\n")
        fh.write("\t\t    metrics_map.insert(p1);\n")
        fh.write("\t\t    obj->counter.Increment();\n")
        fh.write("\t\t}\n")
        fh.write("\t\t}\n")
        fh.write("\t\tbreak;\n")
        fh.write("\t}\n")
    fh.write("\tdefault:\n");
    fh.write("\t\tbreak;\n");
    fh.write("\t}\n")
    fh.write("}\n")
    fh.write("\n\n")
      
def add_decrement_api(fh):
    fh.write("\n\n")
    fh.write("void " + module_name + "::" + "decrement("+module_name+"Counter name,std::map<std::string,std::string> labels)\n") 
    fh.write("{\n")
    fh.write("\tswitch(name) {\n")
    for gauge in gauges_family_object_list:
      for metric in gauge.gaugeMetricList:
        labels = ""
        index = 0
        for l in metric.labeldict:
          if index != 0:
            labels += ","
          for k in l:
             v = l[k]
             labels += "\"" + k + "\"" + ",\"" + v + "\"" 
          index += 1

        fh.write("\tcase "+module_name+"Counter::" + metric.enum_name + ":\n")
        fh.write("\t{\n")
        fh.write("\t\t" + gauge.moduleStatsMember + "->" + metric.gauge_name + ".Decrement();\n")
        fh.write("\t\tfor(auto it = labels.begin(); it != labels.end(); it++) {\n")
        fh.write("\t\tstd::cout<<\"label - (\"<<it->first<<\",\"<<it->second<<\")\"<<std::endl;\n")
        fh.write("\t\tstruct Node s1 = {name, it->first, it->second};\n")
        fh.write("\t\tauto it1 = metrics_map.find(s1);\n")
        fh.write("\t\tif(it1 != metrics_map.end()) {\n")
        fh.write("\t\t    "+gauge.family +"_DynamicMetricObject *obj = static_cast<"+gauge.family+"_DynamicMetricObject *>(it1->second);\n")
        fh.write("\t\t    obj->gauge.Decrement();\n")
        fh.write("\t\t} else {\n")
        fh.write("\t\t    "+gauge.family+"_DynamicMetricObject *obj = " + gauge.moduleStatsMember + "->add_dynamic(" + labels + ",it->first, it->second);\n")
        fh.write("\t\t    auto p1 = std::make_pair(s1, obj);\n")
        fh.write("\t\t    metrics_map.insert(p1);\n")
        fh.write("\t\t    obj->gauge.Decrement();\n")
        fh.write("\t\t}\n")
        fh.write("\t\t}\n")
        fh.write("\t\tbreak;\n")
        fh.write("\t}\n")

    fh.write("\tdefault:\n");
    fh.write("\t\tbreak;\n");
    fh.write("\t}\n")
    fh.write("}\n")
    fh.write("\n\n")
 
def add_test_main_function(fh):
    fh.write("#ifdef MME_TEST_PROMETHEUS \n")
    fh.write("#include <unistd.h>\n")
    fh.write("int main() {\n")
    fh.write("\tstd::thread prom(" + module_name + "SetupPrometheusThread);\n")
    fh.write("\tprom.detach();\n")
    fh.write("\twhile(1) {\n")
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_NUM_UE_SUB_STATE_ACTIVE);\n") 
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_NUM_UE_SUB_STATE_IDLE);\n") 
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE, {{\"enb\",\"1.1.1.2\"}});\n")
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_SECURITY_MODE_RESPONSE);\n")
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_MSG_RX_NAS_AUTHENTICATION_RESPONSE);\n") 
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_SUCCESS);\n")
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_SUCCESS);\n")
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_FAILURE);\n")
    fh.write("\tmmeStats::Instance()->increment(mmeStatsCounter::MME_PROCEDURES_ATTACH_PROC_RESULT_FAILURE, {{\"failure_reason\", \"CSRsp_fail\"}});\n")
    fh.write("\tsleep(1);\n")
    fh.write("\t}\n")
    fh.write("}\n")
    fh.write("#endif\n")

def create_cpp_file():
    fname = module_name+"PromClient.cpp"
    cpp_file = open(fname, 'w')
    add_copyright(cpp_file)
    add_stl_header(cpp_file)
    add_prom_client_setup_function(cpp_file);
    add_module_stats_class_defination(cpp_file)
    add_singleton_obect_create(cpp_file)
    add_gauge_class_defination(cpp_file)
    add_counter_class_defination(cpp_file)
    add_increment_api(cpp_file)
    add_decrement_api(cpp_file)
    add_test_main_function(cpp_file)    

parse_json_file()
create_header_file()
create_cpp_file()


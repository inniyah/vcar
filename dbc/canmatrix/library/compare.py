#!/usr/bin/env python

#Copyright (c) 2013, Eduard Broecker 
#All rights reserved.
#
#Redistribution and use in source and binary forms, with or without modification, are permitted provided that
# the following conditions are met:
#
#    Redistributions of source code must retain the above copyright notice, this list of conditions and the
#    following disclaimer.
#    Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
#    following disclaimer in the documentation and/or other materials provided with the distribution.
#
#THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
#WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
#DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
#CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
#DAMAGE.

from canmatrix import * 


class compareResult:
	def __init__(self, result= None, type= None, ref= None, changes = None):
		# equal, added, deleted, changed
		self._result = result
		# db, bu, frame, signal, attribute
		self._type = type
		#reference to related object
		self._ref = ref
		self._changes = changes
		self._children = []
	def addChild(self, child):
		self._children.append(child)

def propagateChanges(res):
	change = 0
	for child in res._children:
		change += propagateChanges(child)
	if change != 0:
		res._result = "changed"
	if res._result != "equal":
		return 1
	else:
		return 0

def compareDb(db1, db2, ignore = None):
	result = compareResult() 
	for f1 in db1._fl._list:
		f2 = db2.frameById(f1._Id)
		if f2 is None:
			result.addChild(compareResult("deleted", "FRAME", f1))
		else:
			result.addChild(compareFrame(f1, f2, ignore))
	for f2 in db2._fl._list:
		f1 = db1.frameById(f2._Id)
		if f1 is None:
			result.addChild(compareResult("added", "FRAME", f2))

	if ignore is not None and "ATTRIBUTE" in ignore and ignore["ATTRIBUTE"] == "*":
		pass
	else:
		result.addChild(compareAttributes(db1, db2, ignore))

		
	for bu1 in db1._BUs._list:
		bu2 = db2.boardUnitByName(bu1._name)
		if bu2 is None:
			result.addChild(compareResult("deleted", "ecu", bu1))
		else:
			result.addChild(compareBu(bu1, bu2, ignore))
	for bu2 in db2._BUs._list:
		bu1 = db1.boardUnitByName(bu2._name)
		if bu1 is None:
			result.addChild(compareResult("added", "ecu", bu2))


	
	if ignore is not None and "DEFINE" in ignore and ignore["DEFINE"] == "*":
		pass
	else:
		result.addChild(compareDefineList(db1._globalDefines, db2._globalDefines))

		temp = compareDefineList(db1._buDefines, db2._buDefines)
		temp._type = "ECU Defines"
		result.addChild(temp)

		temp = compareDefineList(db1._frameDefines, db2._frameDefines)
		temp._type = "Frame Defines"
		result.addChild(temp)

		temp = compareDefineList(db1._signalDefines, db2._signalDefines)
		temp._type = "Signal Defines"
		result.addChild(temp)

	for vt1 in db1._valueTables:
		if vt1 not in db2._valueTables:
			result.addChild(compareResult("deleted", "valuetable " + vt1, db1._valueTables))
		else:
			result.addChild(compareValueTable(db1._valueTables[vt1], db2._valueTables[vt1]))
			
			
	for vt2 in db2._valueTables:
		if vt2 not in db1._valueTables:
			result.addChild(compareResult("added", "valuetable " + vt2, db2._valueTables))

	propagateChanges(result)

	return result

def compareValueTable(vt1, vt2):
	result = compareResult("equal", "Valuetable", vt1)
	for value in vt1:
		if value not in vt2:
			result.addChild(compareResult("removed", "Value " + str(value), vt1[value]))
		elif vt1[value] != vt2[value]:
			result.addChild(compareResult("changed", "Value " + str(value) + " " + str(vt1[value]), [vt1[value], vt2[value]]))	
	for value in vt2:
		if value not in vt1:
			result.addChild(compareResult("added", "Value " + str(value), vt2[value]))
	return result

def compareSignalGroup(sg1, sg2):
	result = compareResult("equal", "SignalGroup", sg1)

	if sg1._name != sg2._name:
		result.addChild(compareResult("changed", "SignalName", [sg1._name, sg2._name] ))
	if sg1._Id != sg2._Id:
		result.addChild(compareResult("changed", "SignalName", [str(sg1._Id), str(sg2._Id)] ))

	if sg1._members == None or sg2._members == None:
		print "Strange - sg wo members???"
		return result
	for member in sg1._members:
		if sg2.byName(member._name) is None:
			result.addChild(compareResult("deleted", str(member._name), member))
	for member in sg2._members:
		if sg1.byName(member._name) is None:
			result.addChild(compareResult("added", str(member._name), member))
	return result			

	
def compareDefineList(d1list, d2list):
	result = compareResult("equal", "DefineList", d1list)
	for definition in d1list:
		if definition not in d2list:
			result.addChild(compareResult("deleted", "Define" + str(definition), d1list))
		else:
			d2 = d2list[definition]
			d1 = d1list[definition]
			if d1._definition != d2._definition:
				result.addChild(compareResult("changed", "Definition", d1._definition, [d1._definition, d2._definition] ))

			if d1._defaultValue != d2._defaultValue:
				result.addChild(compareResult("changed", "DefaultValue", d1._definition, [d1._defaultValue, d2._defaultValue] ))
	for definition in d2list:
		if definition not in d1list:
			result.addChild(compareResult("added", "Define" + str(definition), d2list))
	return result
	
def compareAttributes(ele1, ele2, ignore = None):
	result = compareResult("equal", "ATTRIBUTES", ele1)	
	if ignore is not None and "ATTRIBUTE" in ignore and (ignore["ATTRIBUTE"] == "*" or ignore["ATTRIBUTE"] == ele1):		
		return result 
	for attribute in ele1._attributes:
		if attribute not in ele2._attributes:
			result.addChild(compareResult("deleted", str(attribute), ele1._attributes[attribute]))
		elif ele1._attributes[attribute] != ele2._attributes[attribute]:
			result.addChild(compareResult("changed", str(attribute), ele1._attributes[attribute], 	 [ ele1._attributes[attribute] , ele2._attributes[attribute]] ))

	for attribute in ele2._attributes:
		if attribute not in ele1._attributes:
			result.addChild(compareResult("added", str(attribute), ele2._attributes[attribute]))
	return result
			
def compareBu(bu1, bu2, ignore=None):
	result = compareResult("equal", "ECU", bu1)

	if bu1._comment != bu2._comment:
		result.addChild(compareResult("changed", "ECU", bu1, [ bu1._comment,  bu2._comment]))

	if ignore is not None and "ATTRIBUTE" in ignore and ignore["ATTRIBUTE"] == "*":
		pass
	else:
		result.addChild(compareAttributes(bu1, bu2, ignore))
	return result
	
def compareFrame(f1, f2, ignore= None):
	result = compareResult("equal", "FRAME", f1)

	for s1 in f1._signals:
		s2 = f2.signalByName(s1._name)
		if not s2:
			result.addChild(compareResult("deleted", "SIGNAL", s1))
		else:
			result.addChild(compareSignal(s1, s2, ignore))

	if f1._name != f2._name:
		result.addChild(compareResult("changed", "Name", f1, [f1._name, f2._name]))
	if f1._Size != f2._Size:
		result.addChild(compareResult("changed", "dlc", f1, ["dlc: %d" % f1._Size, "dlc: %d" % f2._Size]))
	if f1._extended != f2._extended:
		result.addChild(compareResult("changed", "FRAME", f1, ["extended-Flag: %d" % f1._extended, "extended-Flag: %d" % f2._extended]))
	if f2._comment == None:
		f2._comment = ""	
	if f1._comment == None:
		f1._comment = ""	
	if f1._comment != f2._comment:
		result.addChild(compareResult("changed", "FRAME", f1, ["comment: " +  f1._comment , "comment: " +  f2._comment]))
		
	for s2 in f2._signals:
		s1 = f1.signalByName(s2._name)
		if not s1:
			result.addChild(compareResult("added", "SIGNAL", s2))

	if ignore is not None and "ATTRIBUTE" in ignore and ignore["ATTRIBUTE"] == "*":		
		pass
	else:
		result.addChild(compareAttributes(f1, f2, ignore))

	for transmitter in f1._Transmitter:
		if transmitter not in f2._Transmitter:
			result.addChild(compareResult("removed", "Frame-Transmitter", f1))
	for transmitter in f2._Transmitter:
		if transmitter not in f1._Transmitter:
			result.addChild(compareResult("added", "Frame-Transmitter", f2))

	for sg1 in f1._SignalGroups:
		sg2 = f2.signalGroupbyName(sg1._name)
		if sg2 is None:
			result.addChild(compareResult("removed", "Signalgroup", sg1))
		else:
			result.addChild(compareSignalGroup(sg1,sg2))

	for sg2 in f2._SignalGroups:
		if f1.signalGroupbyName(sg2._name) is None:
			result.addChild(compareResult("added", "Signalgroup", sg1))
	return result

def compareSignal(s1,s2, ignore = None):
	result = compareResult("equal", "SIGNAL", s1)

	if s1._startbit != s2._startbit:
		result.addChild(compareResult("changed", "startbit", s1, [" %d" % s1._startbit, " %d" % s2._startbit]))
	if s1._signalsize != s2._signalsize:
		result.addChild(compareResult("changed", "signalsize", s1, [" %d" % s1._signalsize, " %d" % s2._signalsize]))
	if s1._factor != s2._factor:
		result.addChild(compareResult("changed", "factor", s1, [s1._factor, s2._factor]))
	if s1._offset != s2._offset:
		result.addChild(compareResult("changed", "offset", s1, [ s1._offset, s2._offset]))
	if s1._min != s2._min:
		result.addChild(compareResult("changed", "min", s1, [ s1._min, s2._min]))
	if s1._max != s2._max:
		result.addChild(compareResult("changed", "max", s1, [ s1._max,  s2._max]))
	if s1._byteorder != s2._byteorder:
		result.addChild(compareResult("changed", "byteorder", s1, [" %d" % s1._byteorder, " %d" % s2._byteorder]))
	if s1._valuetype != s2._valuetype:
		result.addChild(compareResult("changed", "valuetype", s1, [" %d" % s1._valuetype, " %d" % s2._valuetype]))
	if s1._multiplex != s2._multiplex:
		result.addChild(compareResult("changed", "multiplex", s1, [str(s1._multiplex), str(s2._multiplex)]))
	if s1._unit != s2._unit:
		result.addChild(compareResult("changed", "unit", s1, [ s1._unit,  s2._unit]))
	if s1._comment is not None and s2._comment is not None and s1._comment != s2._comment:
		if s1._comment.replace("\n"," ") != s2._comment.replace("\n"," "):
			result.addChild(compareResult("changed", "comment", s1, [ s1._comment,  s2._comment]))
		else:
			result.addChild(compareResult("changed", "comment", s1, ["only whitespaces differ", ""]))


	for reciever in s1._reciever:
		if reciever not in s2._reciever:
			result.addChild(compareResult("removed", "Reciever " + reciever, s1._reciever))

	for reciever in s2._reciever:
		if reciever not in s1._reciever:
			result.addChild(compareResult("added", "Reciever " + reciever, s1._reciever))

	if ignore is not None and "ATTRIBUTE" in ignore and ignore["ATTRIBUTE"] == "*":
		pass
	else:
		result.addChild(compareAttributes(s1, s2, ignore))

	result.addChild(compareValueTable(s1._values,s2._values))
			
	return result

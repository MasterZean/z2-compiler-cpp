#include "BaseCompiler.h"
#include "Scanner.h"

int BaseCompiler::LookUpClass(ZSource& source, const String& className) {
	int classIndex = -1;
	
	int pointIndex = className.Find('.');
	
	if (pointIndex == -1) {
		// single component class name
		classIndex = LookUpClassInReferences(source, className);
	}
	else {
		// fully qualified class
		classIndex = LookUpQualifiedClass(className);
	}
	
	return classIndex;
}

int BaseCompiler::LookUpQualifiedClass(const String& className) {
	ASSERT(className.Find('.') != -1);
	
	int classIndex = ass.Classes.Find(className);
	
	if (classIndex == -1 || ass.Classes[classIndex].IsDefined == false) {
		int sourceIndex = LookUp.Find(className);
		if (sourceIndex == -1)
			return -1;
		
		ZSource* source = FindSource(LookUp[sourceIndex]);
		if (source == nullptr)
			return -1;

		LoadSource(*source);
		
		// double check: guard against unexpected errors in lookup and class loading
		classIndex = ass.Classes.Find(className);
		if (classIndex == -1)
			return -1;
	}
	
	return classIndex;
}

int BaseCompiler::LookUpClassInReferences(ZSource& source, const String& className) {
	String newClassName = ".";
	newClassName << className;
	
	for (int i = 0; i < source.References.GetCount(); i++)
		if (source.References[i].EndsWith(newClassName))
			return LookUpQualifiedClass(source.References[i]);
	
	return -1;
}

ZSource* BaseCompiler::FindSource(const String& aSourcePath) {
	int pakIndex = -1;
	
	for (int i = 0; i < packages.GetCount(); i++) {
		ZPackage& pak = packages[i];
		if (aSourcePath.StartsWith(pak.Path)) {
			String relPath = aSourcePath.Mid(pak.Path.GetLength());

			pakIndex = pak.Files.Find(relPath);
			if (pakIndex != -1)
				return &packages[i].Files[pakIndex];
		}
	}
	
	return nullptr;
}

ZSource& BaseCompiler::LoadSource(ZSource& source) {
	if (source.IsScaned)
		return source;
	
	String fullPath = source.Package->Path + source.Path;
	//Cout() << "Loading: " << fullPath << "\n";
	filesOpened++;
	source.Data = LoadFile(fullPath);
	
	Scanner scanner(source, Platform == WINDOWS32);
	scanner.Scan();
	source.AddStdClassRefs();
	source.IsScaned = true;
	
	ass.AddSource(source);
	
	for (int i = 0; i < source.ClassPrototypes.GetCount(); i++) {
		ZClass& cls = source.ClassPrototypes[i];
		String fullName = cls.Scan.Namespace;
		fullName << cls.Scan.Name;
		//LOG(fullName + " " + fullPath);
		int cc = LookUp.GetCount();
		int ii = LookUp.FindAdd(fullName, fullPath);
		if (ii >= cc)
			ass.AddClassCount(cls.Scan.Name);
	}
	
	for (int k = 0; k < source.Aliases.GetCount(); k++) {
		//LOG(source.Aliases[k].Namespace + source.Aliases[k].Name + " " + fullPath);
		String fullName = source.Aliases[k].Namespace;
		fullName << source.Aliases[k].Name;
		int cc = LookUp.GetCount();
		int ii = LookUp.FindAdd(fullName, fullPath);
		if (ii >= cc)
			ass.AddClassCount(source.Aliases[k].Name);
	}
	
	return source;
}
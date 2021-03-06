#include <z2clib/Assembly.h>
#include <z2clib/Source.h>

String s[] = {
	"sys.core.lang.Def",
	"sys.core.lang.Ptr",
	"sys.core.lang.System",
	
	"sys.core.lang.Bool",
	
	"sys.core.lang.Small",
	"sys.core.lang.Byte",
	"sys.core.lang.Short",
	"sys.core.lang.Word",
	"sys.core.lang.Int",
	"sys.core.lang.DWord",
	"sys.core.lang.Long",
	"sys.core.lang.QWord",
	
	"sys.core.lang.Float",
	"sys.core.lang.Double",
	
	"sys.core.lang.PtrSize",
	"sys.core.lang.Char",
	
	"sys.core.lang.String",
	
	"sys.core.lang.CArray",
	"sys.core.lang.Vector",

	"sys.core.lang.Intrinsic",
	"sys.core.lang.Slice",
	"sys.core.lang.ReadSlice",
};

void ZSource::AddStdClassRefs() {
	for (int i = 0; i < __countof(s); i++)
		AddReference(s[i]);
}

void ZSource::AddReference(const String& ns) {
	int i = FindIndex(References, ns);
	if (i == -1) {
		References.Add(ns);
		ReferencePos.Add(Point(1, 1));
	}
}

ZClass& ZSource::AddClass(const String& name, const String& nameSpace, const Point& pnt) {
	String fullName = nameSpace;
	fullName << name;

	ZClass& c = ClassPrototypes.Add(fullName);
	c.Scan.Name = name;
	c.BackendName = name;
	c.Position = pnt;
	c.Scan.Namespace = nameSpace;
	c.ParamType = &c;
	c.IsDefined = true;
	
	return c;
}

int ZSource::FindClassReference(const String& shortName, int start) {
	for (int i = start; i < References.GetCount(); i++) {
		const String& rf = References[i];
		
		int dotPos = rf.GetCount() - shortName.GetCount() - 1;
		
		if (rf.GetCount() > shortName.GetCount() && rf[dotPos] == '.') {
			const char* s = rf.Begin() + dotPos + 1;
			
			if (shortName == s)
				return i;
		}
	}
	
	return -1;
}


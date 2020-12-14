

def generate_variant_components():
    f = open("./components/variant_component.gen.h", "w")
    f.write("/* THIS FILE IS GENERATED DO NOT EDIT */\n")
    f.write("#ifndef VARIANT_COMPONENT_GENERATED_GEN_H\n")
    f.write("#define VARIANT_COMPONENT_GENERATED_GEN_H\n")
    f.write("#include \"component.h\"\n")

    for i in range(2000):# It's already too much...
        f.write("class Variant"+str(i)+"Component : public godex::Component {\n")
        f.write("	COMPONENT(Variant"+str(i)+"Component, DenseVector)\n")
        f.write("	Variant variants[0];\n")
        f.write("public:\n")
        f.write("	Variant"+str(i)+"Component() {}\n")
        f.write("};\n")

    f.write("#endif // VARIANT_COMPONENT_GENERATED_GEN_H\n")
    f.close()

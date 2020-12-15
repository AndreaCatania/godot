#include "variant_component.h"

DynamicComponentInfo::DynamicComponentInfo() {
}

Storage *DynamicComponentInfo::create_storage() {
	switch (storage_type) {
		case StorageType::DENSE_VECTOR:
			// Creates DenseVector storage.
			switch (properties.size()) {
				case 0:
					return memnew(DenseVector<VariantComponent<0>>);
				case 1:
					return memnew(DenseVector<VariantComponent<1>>);
				case 2:
					return memnew(DenseVector<VariantComponent<2>>);
				case 3:
					return memnew(DenseVector<VariantComponent<3>>);
				case 4:
					return memnew(DenseVector<VariantComponent<4>>);
				case 5:
					return memnew(DenseVector<VariantComponent<5>>);
				case 6:
					return memnew(DenseVector<VariantComponent<6>>);
				case 7:
					return memnew(DenseVector<VariantComponent<7>>);
				case 8:
					return memnew(DenseVector<VariantComponent<8>>);
				case 9:
					return memnew(DenseVector<VariantComponent<9>>);
				case 10:
					return memnew(DenseVector<VariantComponent<10>>);
				case 11:
					return memnew(DenseVector<VariantComponent<11>>);
				case 12:
					return memnew(DenseVector<VariantComponent<12>>);
				case 13:
					return memnew(DenseVector<VariantComponent<13>>);
				case 14:
					return memnew(DenseVector<VariantComponent<14>>);
				case 15:
					return memnew(DenseVector<VariantComponent<15>>);
				case 16:
					return memnew(DenseVector<VariantComponent<16>>);
				case 17:
					return memnew(DenseVector<VariantComponent<17>>);
				case 18:
					return memnew(DenseVector<VariantComponent<18>>);
				case 19:
					return memnew(DenseVector<VariantComponent<19>>);
				case 20:
					return memnew(DenseVector<VariantComponent<20>>);
			}
		case StorageType::NONE:
		default:
			return nullptr;
	}
}

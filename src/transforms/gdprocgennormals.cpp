#include "transforms/gdprocgennormals.h"

using namespace godot;

void GDProcGenNormals::_register_methods() {

}

String GDProcGenNormals::get_type_name() {
	return "Generate normals";
}

void GDProcGenNormals::_init() {

}

bool GDProcGenNormals::update(bool p_inputs_updated, const Array &p_inputs) {
	bool updated = must_update;
	must_update = false;

	if (updated || p_inputs_updated) {
		printf("Generate normals\n");

		int num_vertices = 0;
		PoolVector3Array vertices;
		int num_indices = 0;
		PoolIntArray indices;

		int input_count = p_inputs.size();
		if (input_count > 0) {
			if (p_inputs[0].get_type() == Variant::POOL_VECTOR3_ARRAY) {
				vertices = p_inputs[0];
				num_vertices = vertices.size();
			}
		}

		if (input_count > 1) {
			if (p_inputs[1].get_type() == Variant::POOL_INT_ARRAY) {
				indices = p_inputs[1];
				num_indices = indices.size();
			}
		}

		if ((num_vertices > 0) && (num_indices > 0)) {
			normals.resize(num_vertices);

			{
				PoolVector3Array::Write vertices_write = vertices.write();

				// normalize our vertices
				for (int n = 0; n < num_vertices; n++) {
					vertices_write[n].normalize();
				}
			}

			PoolVector3Array::Write w = normals.write();
			PoolVector3Array::Read vertices_read = vertices.read();
			PoolIntArray::Read indices_read = indices.read();

			// lets reset our normals
			for (int n = 0; n < num_vertices; n++) {
				w[n] = Vector3(0.0, 0.0, 0.0);
			}

			// add our normals
			for (int i = 0; i < num_indices; i+=3) {
				int a = indices_read[i];
				int b = indices_read[i + 1];
				int c = indices_read[i + 2];

				// get the normalized vertices for this face
				Vector3 v1 = vertices_read[a];
				Vector3 v2 = vertices_read[b];
				Vector3 v3 = vertices_read[c];

				// calculate our vectors
				v2 -= v1;
				v3 -= v1;

				// cross product to get our normal
				Vector3 n = v3.cross(v2);

				// and add our normal to the normals for our face
				w[a] += n;
				w[b] += n;
				w[c] += n;
			}

			// now normalise our normals
			for (int n = 0; n < num_vertices; n++) {
				w[n].normalize();
			}
		} else {
			normals.resize(0);
		}
	}

	return updated;
}

int GDProcGenNormals::get_input_connector_count() const {
	return 2;
}

Variant::Type GDProcGenNormals::get_input_connector_type(int p_slot) const {
	if (p_slot == 0) {
		return Variant::POOL_VECTOR3_ARRAY;
	} else if (p_slot == 1) {
		return Variant::POOL_INT_ARRAY;
	}

	return Variant::NIL;
}

const String GDProcGenNormals::get_input_connector_name(int p_slot) const {
	if (p_slot == 0) {
		return "Vertices";
	} else if (p_slot == 1) {
		return "Indices";
	}

	return "";
}

int GDProcGenNormals::get_output_connector_count() const {
	return 1;
}

Variant::Type GDProcGenNormals::get_output_connector_type(int p_slot) const {
	return Variant::POOL_VECTOR3_ARRAY;
}

const String GDProcGenNormals::get_output_connector_name(int p_slot) const {
	return "Normals";
}

const Variant GDProcGenNormals::get_output(int p_slot) const {
	return Variant(normals);
}
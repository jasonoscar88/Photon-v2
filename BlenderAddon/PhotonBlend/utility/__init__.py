from mathutils import Vector, Quaternion

import os
from pathlib import Path


def to_photon_vec3(bvec3):
	return Vector((bvec3.y, bvec3.z, bvec3.x))


def to_photon_quat(bquat):
	return Quaternion((bquat.w, bquat.y, bquat.z, bquat.x))


def create_folder(folder_path):
	path = Path(folder_path)
	path.mkdir(parents = True, exist_ok = True)


def get_filename(file_path):
	return os.path.basename(file_path)


def get_filename_without_ext(file_path):
	filename = os.path.basename(file_path)
	return os.path.splitext(filename)[0]


def get_folder_path(file_path):
	folder_path = os.path.split(file_path)[0]
	if not folder_path.endswith(path_separator()):
		folder_path += path_separator()
	return folder_path


def get_appended_path(parent_path, child_path):
	return os.path.join(parent_path, child_path)


def path_separator():
	return os.sep

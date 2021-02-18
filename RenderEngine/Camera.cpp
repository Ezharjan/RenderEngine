#include "Camera.h"


namespace RenderEngine {

	void Camera::Update()
	{
		model = Matrix4f::getIdentityMatrix();
		model = model * Matrix4f::getRotationMatrix(Vector3(1.0f, 0.0f, 0.0f), TO_RADIANS(modelRot.getX()));
		model = model * Matrix4f::getRotationMatrix(Vector3(0.0f, 1.0f, 0.0f), TO_RADIANS(modelRot.getY()));

		invModel = Matrix4f::getMatrixInvert(model);

		UpdateViewMatrix();
		view.makeTranslation(trans);
		view = view * Matrix4f::getRotationMatrix(Vector3(1.0f, 0.0f, 0.0f), TO_RADIANS(viewRot.getX()));
		view = view * Matrix4f::getRotationMatrix(Vector3(0.0f, 1.0f, 0.0f), TO_RADIANS(viewRot.getY()));

		UpdateProjectionMatirx();
	}

	void Camera::UpdateViewMatrix()
	{
		Vector4 z_axis = target - position;
		z_axis.Normalize();
		Vector4 x_axis = up ^ z_axis;
		x_axis.Normalize();
		Vector4 y_axis = z_axis ^ x_axis;

		view.matrix[0][0] = x_axis.getX();
		view.matrix[1][0] = x_axis.getY();
		view.matrix[2][0] = x_axis.getZ();
		view.matrix[3][0] = x_axis * position * (-1);

		view.matrix[0][1] = y_axis.getX();
		view.matrix[1][1] = y_axis.getY();
		view.matrix[2][1] = y_axis.getZ();
		view.matrix[3][1] = (y_axis)* position * (-1);

		view.matrix[0][2] = z_axis.getX();
		view.matrix[1][2] = z_axis.getY();
		view.matrix[2][2] = z_axis.getZ();
		view.matrix[3][2] = z_axis * position * (-1);

		view.matrix[0][3] = view.matrix[1][3] = view.matrix[2][3] = 0.0f;
		view.matrix[3][3] = 1.0f;

		staticView = view;
	}

	void Camera::UpdateProjectionMatirx()
	{
		float cotHalfFovY = 1.0f / (float)tan(FovY * 0.5f);
		proj.matrix[0][0] = (float)(cotHalfFovY / aspect);
		proj.matrix[1][1] = (float)(cotHalfFovY);
		proj.matrix[2][2] = (Near + Far) / (Far - Near);
		proj.matrix[3][2] = -2 * Near*Far / (Far - Near);
		proj.matrix[2][3] = 1.0f;
	}

}

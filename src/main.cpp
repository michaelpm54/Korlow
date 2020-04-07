#include "render/opengl.h"

#include <stdexcept>
#include <string>

#include <QApplication>
#include <QOffscreenSurface>

#include "emulator.h"

int main(int argc, char *argv[])
{
	try
	{
		QSurfaceFormat format;
		format.setStencilBufferSize(8);
		format.setVersion(4, 5);
		format.setProfile(QSurfaceFormat::CoreProfile);
		QSurfaceFormat::setDefaultFormat(format);

		QApplication app(argc, argv);

		auto emu = Emulator();

		return app.exec();
	}
	catch (const std::runtime_error& e)
	{
		fprintf(stderr, "%s", e.what());
		return 1;
	}
	catch (const GLubyte* str)
	{
		fprintf(stderr, "%s", str);
		return 1;
	}
}

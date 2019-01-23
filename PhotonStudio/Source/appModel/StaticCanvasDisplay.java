package appModel;

import java.awt.image.BufferedImage;
import java.io.IOException;
import java.nio.file.Path;
import java.util.HashSet;
import java.util.Set;

import javax.imageio.ImageIO;

import javafx.beans.value.ChangeListener;
import javafx.embed.swing.SwingFXUtils;
import javafx.event.EventHandler;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.image.Image;
import javafx.scene.image.PixelReader;
import javafx.scene.image.PixelWriter;
import javafx.scene.image.WritableImage;
import javafx.scene.input.MouseEvent;
import javafx.scene.paint.Color;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.Rectangle;
import util.Vector2f;
import util.Vector3f;

public class StaticCanvasDisplay extends Display
{
	private WritableImage m_image;
	private Canvas        m_canvas;
	private Set<Rectangle> m_updatingRegions;
	
	public StaticCanvasDisplay()
	{
		this(1, 1);
	}
	
	public StaticCanvasDisplay(int widthPx, int heightPx)
	{
		super();
		
		m_image  = new WritableImage(widthPx, heightPx);
		m_canvas = new Canvas(widthPx, heightPx);
		
		ChangeListener<Number> sizeChangeListener = (ovservable, oldValue, newValue) ->
			getDisplayView().showDisplayResolution((int)(m_canvas.getWidth() + 0.5), (int)(m_canvas.getHeight() + 0.5));
		m_canvas.widthProperty().addListener(sizeChangeListener);
		m_canvas.heightProperty().addListener(sizeChangeListener);
		
		ChangeListener<Number> redrawListener = (ovservable, oldValue, newValue) ->
			{clearFrame(); drawFrame();};
		m_canvas.widthProperty().addListener(redrawListener);
		m_canvas.heightProperty().addListener(redrawListener);
		
		m_canvas.setOnMouseMoved(new EventHandler<MouseEvent>()
		{
			@Override
			public void handle(MouseEvent event)
			{
				Vector2f canvasCoordPx = new Vector2f((float)event.getX(), (float)event.getY());
				Vector2f frameCoordPx  = getFrameCoordPx(canvasCoordPx);
				
				getDisplayView().showCursorCoord(
					Math.min((int)frameCoordPx.x, getFrameWidthPx() - 1), 
					Math.min((int)frameCoordPx.y, getFrameHeightPx() - 1));
				
				int x = (int)frameCoordPx.x;
				int y = (int)(m_image.getHeight() - frameCoordPx.y);
				x = Math.min(x, getFrameWidthPx() - 1);
				y = Math.min(y, getFrameHeightPx() - 1);
				
				Color color = m_image.getPixelReader().getColor(x, y);
				getDisplayView().showCursorColor(
					(int)(color.getRed() * 255.0), 
					(int)(color.getGreen() * 255.0), 
					(int)(color.getBlue() * 255.0));
			}
		});
		
		getDisplayView().showFrameResolution(widthPx, heightPx);
		getDisplayView().showDisplayResolution(widthPx, heightPx);
		
		m_updatingRegions = new HashSet<>();
	}
	
	@Override
	public void loadFrame(FrameRegion frame)
	{
		copyFrameToImage(frame);
		m_updatingRegions.clear();
	}
	
	@Override
	public void loadFrame(FrameRegion frame, FrameStatus status)
	{
		final Rectangle region = frame.getRegion();
		
		if(status == FrameStatus.UPDATING)
		{
			copyFrameToImage(frame);
			m_updatingRegions.add(region);
			
//			final int minX = region.x;
//			final int maxX = region.x + region.w - 1;
//			final int minY = getFrameHeightPx() - (region.y + region.h);
//			final int maxY = getFrameHeightPx() - region.y - 1;
//			
//			final PixelWriter pixelWriter = m_image.getPixelWriter();
//			final Color rectColor = new Color(0, 1, 0, 1);
//			
//			for(int x = minX; x <= maxX; ++x)
//			{
//				pixelWriter.setColor(x, minY, rectColor);
//				pixelWriter.setColor(x, maxY, rectColor);
//			}
//			
//			for(int y = minY + 1; y <= maxY - 1; ++y)
//			{
//				pixelWriter.setColor(minX, y, rectColor);
//				pixelWriter.setColor(maxX, y, rectColor);
//			}
		}
		else if(status == FrameStatus.FINISHED)
		{
			copyFrameToImage(frame);
			m_updatingRegions.remove(region);
		}
	}
	
	@Override
	public void drawFrame()
	{
		Vector2f drawResPx = getFittedDrawResPx();
		Vector2f originPx  = getCenteredOriginPx(drawResPx);
		
		GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.drawImage(
			m_image, 
			originPx.x, (m_canvas.getHeight() - originPx.y) - drawResPx.y, 
			drawResPx.x, drawResPx.y);
		
		g.setFill(new Color(0, 1, 0, 1));
		for(Rectangle region : m_updatingRegions)
		{
			Rectangle r = new Rectangle(
				(int)(originPx.x + ((double)region.x / getFrameWidthPx()) * drawResPx.x),
				(int)(originPx.y + (1.0 - (double)(region.y + region.h) / getFrameHeightPx()) * drawResPx.y),
				(int)Math.ceil((double)region.w / getFrameWidthPx() * drawResPx.x),
				(int)Math.ceil((double)region.h / getFrameHeightPx() * drawResPx.y));
			
			g.fillRect(r.x, r.y, r.w, 1);
			g.fillRect(r.x, r.y + r.h - 1, r.w, 1);
			g.fillRect(r.x, r.y, 1, r.h);
			g.fillRect(r.x + r.w - 1, r.y, 1, r.h);
		}
		
		getDisplayView().showZoom(getDrawnScale(drawResPx) * 100.0f);
	}
	
	@Override
	public void clearFrame()
	{
		GraphicsContext g = m_canvas.getGraphicsContext2D();
		g.setFill(Color.DARKBLUE);
		g.fillRect(0, 0, m_canvas.getWidth(), m_canvas.getHeight());
	}
	
	@Override
	public BufferedImage getImage()
	{
		return SwingFXUtils.fromFXImage(m_image, null);
	}
	
	public Canvas getCanvas()
	{
		return m_canvas;
	}
	
	public int getFrameWidthPx()
	{
		return (int)(m_image.getWidth());
	}
	
	public int getFrameHeightPx()
	{
		return (int)(m_image.getHeight());
	}
	
	private Vector2f getFittedDrawResPx()
	{
		final float canvasWidth       = (float)(m_canvas.getWidth());
		final float canvasHeight      = (float)(m_canvas.getHeight());
		final float canvasAspectRatio = canvasWidth / canvasHeight;
		final float frameAspectRatio  = (float)(m_image.getWidth()) / (float)(m_image.getHeight());
		
		Vector2f resPx = new Vector2f();
		if(frameAspectRatio > canvasAspectRatio)
		{
			resPx.x  = (int)canvasWidth;
			resPx.y = (int)(canvasWidth / frameAspectRatio);
		}
		else
		{
			resPx.x = (int)(canvasHeight * frameAspectRatio);
			resPx.y = (int)canvasHeight;
		}
		
		return resPx;
	}
	
	private Vector2f getCenteredOriginPx(Vector2f drawSizePx)
	{
		return new Vector2f(
			(float)(m_canvas.getWidth() - drawSizePx.x) * 0.5f, 
			(float)(m_canvas.getHeight() - drawSizePx.y) * 0.5f);
	}
	
	private Vector2f getFrameCoordPx(Vector2f canvasCoordPx)
	{
		// FIXME: centered and fitted frame assumed
		Vector2f drawResPx = getFittedDrawResPx();
		Vector2f originPx  = getCenteredOriginPx(drawResPx);
		
		// flipping canvas y
		Vector2f coordPx = new Vector2f(canvasCoordPx.x, (float)m_canvas.getHeight() - canvasCoordPx.y);
		
		// relative to drawed frame
		coordPx = coordPx.sub(originPx);
		
		// normalize and scale to match actual frame size
		Vector2f frameSizePx = new Vector2f((float)m_image.getWidth(), (float)m_image.getHeight());
		coordPx = coordPx.div(drawResPx).mul(frameSizePx);
		
		// clamp to edge
		coordPx.x = Math.max(Math.min(coordPx.x, frameSizePx.x), 0);
		coordPx.y = Math.max(Math.min(coordPx.y, frameSizePx.y), 0);
		
		return coordPx;
	}
	
	private float getDrawnScale(Vector2f drawSizePx)
	{
		// FIXME: retained aspect ratio assumed
		return (float)(drawSizePx.x / m_image.getWidth());
	}
	
	private void copyFrameToImage(FrameRegion frame)
	{
		if(!frame.isValid() || frame.getNumComp() != 3)
		{
			System.err.println("unexpected frame format; unable to load");
			return;
		}
		
		if(m_image.getWidth()  != frame.getFullWidthPx() || 
		   m_image.getHeight() != frame.getFullHeightPx())
		{
			m_image = new WritableImage(frame.getFullWidthPx(), frame.getFullHeightPx());
			
			getDisplayView().showFrameResolution(frame.getFullWidthPx(), frame.getFullHeightPx());
		}
		
		final PixelWriter pixelWriter = m_image.getPixelWriter();
		final Rectangle region = frame.getRegion();
		final int endX = region.x + region.w;
		final int endY = region.y + region.h;
		final Vector3f color = new Vector3f();
		
		for(int y = region.y; y < endY; ++y)
		{
			for(int x = region.x; x < endX; ++x)
			{
				color.set(frame.getRgb(x, y));
				if(!Float.isFinite(color.x) ||
				   !Float.isFinite(color.y) || 
				   !Float.isFinite(color.z))
				{
					System.err.println("color is not finite: " + color);
					color.set(0, 0, 0);
				}
				
				color.clampLocal(0.0f, 1.0f);
				
				int inversedY = frame.getFullHeightPx() - y - 1;
				Color fxColor = new Color(color.x, color.y, color.z, 1.0);
				pixelWriter.setColor(x, inversedY, fxColor);
			}
		}
	}
}

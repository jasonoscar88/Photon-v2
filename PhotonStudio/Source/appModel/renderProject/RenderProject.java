package appModel.renderProject;

import java.io.File;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.Future;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.TimeUnit;

import appModel.Studio;
import appModel.GeneralOption;
import appModel.Project;
import appModel.ShowView;
import javafx.application.Platform;
import javafx.concurrent.Task;
import photonApi.FilmInfo;
import photonApi.Frame;
import photonApi.FrameRegion;
import photonApi.FrameStatus;
import photonApi.ObservableRenderData;
import photonApi.Ph;
import photonApi.PhEngine;
import photonApi.PhFrame;
import photonApi.Rectangle;
import photonApi.RenderState;
import photonApi.Statistics;

public class RenderProject extends Project
{
	private PhEngine m_engine;
	private PhFrame  m_finalFrame;
	private PhFrame  m_transientFrame;
	private Frame    m_localFinalFrame;
	
	private Studio        m_studio;
	private RenderSetting m_renderSetting;
	
	private RenderStatusView         m_renderStatusView;
	private RenderFrameView          m_renderFrameView;
	private ExecutorService          m_projectTaskExecutor;
	private ScheduledExecutorService m_monitorExecutor;
	private List<Future<?>>          m_monitorHandles;
	private RenderFrameQuery         m_renderFrameQuery;
	
	public RenderProject(String projectName, Studio studio)
	{
		super(projectName);
		
		m_engine          = null;
		m_finalFrame      = null;
		m_transientFrame  = null;
		m_localFinalFrame = new Frame();
		
		m_studio        = studio;
		m_renderSetting = new RenderSetting(studio.getGeneralOption());
		
		m_renderStatusView    = new RenderStatusView(){};
		m_renderFrameView     = new RenderFrameView(){};
		m_projectTaskExecutor = null;
		m_monitorExecutor     = null;
		m_monitorHandles      = new ArrayList<>();
		m_renderFrameQuery    = null;
	}
	
	@Override
	protected void createResource()
	{
		m_engine         = new PhEngine(1);
		m_finalFrame     = new PhFrame(0, 0);
		m_transientFrame = new PhFrame(0, 0);
		
		m_projectTaskExecutor = Executors.newSingleThreadExecutor();
		m_monitorExecutor     = Executors.newSingleThreadScheduledExecutor();
	}

	@Override
	protected void decomposeResource()
	{
		// TODO: stop all engine work
		
		m_monitorExecutor.shutdownNow();
		m_projectTaskExecutor.shutdownNow();
		
		m_engine.dispose();
		m_finalFrame.dispose();
		m_transientFrame.dispose();
	}
	
	public void runLoadSceneTask(ShowView before, ShowView after)
	{
		runTask(newLoadSceneTask(), before, after);
	}
	
	public void runRenderTask(ShowView before, ShowView after)
	{
		Task<Void> renderTask = newRenderTask();
		renderTask.setOnRunning((event) -> startMonitoring());
		renderTask.setOnFailed((event) -> stopMonitoring());
		renderTask.setOnSucceeded((event) -> stopMonitoring());
		
		runTask(renderTask, before, after);
	}
	
	public void runUpdateStaticImageTask(ShowView before, ShowView after)
	{
		Task<Void> updateStaticImageTask = newUpdateStaticImageTask();
		updateStaticImageTask.setOnSucceeded((event) -> m_renderFrameView.showFinal(getLocalFinalFrame()));
		
		runTask(updateStaticImageTask, before, after);
	}
	
	public void runTask(Task<Void> task, ShowView before, ShowView after)
	{
		if(before != null)
		{
			m_projectTaskExecutor.submit(new Task<Void>()
			{
				@Override
				protected Void call() throws Exception
				{
					Platform.runLater(() -> before.show());
					return null;
				}
			});
		}
		
		m_projectTaskExecutor.submit(task);
		
		if(after != null)
		{
			m_projectTaskExecutor.submit(new Task<Void>()
			{
				@Override
				protected Void call() throws Exception
				{
					Platform.runLater(() -> after.show());
					return null;
				}
			});
		}
	}
	
	public void asyncGetRendererStatistics(Statistics out_statistics)
	{
		m_engine.asyncGetRendererStatistics(out_statistics);
	}
	
	public FrameStatus asyncGetUpdatedFrame(FrameRegion out_frameRegion)
	{
		return asyncGetUpdatedFrame(Ph.ATTRIBUTE_LIGHT_ENERGY, out_frameRegion);
	}
	
	public FrameStatus asyncGetUpdatedFrame(int attribute, FrameRegion out_frameRegion)
	{
		Rectangle region = new Rectangle();
		FrameStatus status = m_engine.asyncGetUpdatedFrame(attribute, m_transientFrame, region);
		if(status != FrameStatus.INVALID)
		{
			out_frameRegion.set(m_transientFrame.copyRegionRgb(region));
		}
		
		return status;
	}
	
	public RenderState asyncGetRenderState()
	{
		return m_engine.asyncGetRenderState();
	}
	
	public RenderSetting getRenderSetting()
	{
		return m_renderSetting;
	}
	
	public Frame getLocalFinalFrame()
	{
		return m_localFinalFrame;
	}
	
	public FilmInfo getFilmInfo()
	{
		return m_engine.getFilmInfo();
	}
	
	public GeneralOption getGeneralOption()
	{
		return m_studio.getGeneralOption();
	}
	
	public void setRenderStatusView(RenderStatusView view)
	{
		m_renderStatusView = view;
	}
	
	public void setRenderFrameView(RenderFrameView view)
	{
		m_renderFrameView = view;
	}
	
	private void startMonitoring()
	{
		stopMonitoring();
		
		m_monitorHandles.add(
			m_monitorExecutor.scheduleWithFixedDelay(
				newRenderStatusQuery(), 
				0, 1, TimeUnit.SECONDS));
				
		m_renderFrameQuery = new RenderFrameQuery(this, m_renderFrameView);
		m_renderFrameQuery.scheduleAdaptively(m_monitorExecutor);
		
//		m_monitorHandles.add(
//			m_monitorExecutor.scheduleWithFixedDelay(
//				newRenderFrameQuery(), 
//				0, 1, TimeUnit.SECONDS));
	}
	
	private void stopMonitoring()
	{
		for(Future<?> monitorHandle : m_monitorHandles)
		{
			monitorHandle.cancel(true);
		}
		m_monitorHandles.clear();
		
		if(m_renderFrameQuery != null)
		{
			m_renderFrameQuery.cancelAdaptivelyScheduled();
		}
	}
	
	private Task<Void> newRenderTask()
	{
		return new Task<Void>()
		{
			@Override
			protected Void call() throws Exception
			{
				Platform.runLater(() -> 
					getLogView().showLog("rendering scene..."));
			
				m_engine.render();
				
				return null;
			}
		};
	}
	
	private Task<Void> newLoadSceneTask()
	{
		return new Task<Void>()
		{
			@Override
			protected Void call() throws Exception
			{
				String sceneFilePath = getRenderSetting().get(RenderSetting.SCENE_FILE_PATH);
				
				Platform.runLater(() -> 
					getLogView().showLog("loading scene file <" + sceneFilePath + ">..."));
				
				File sceneFile = new File(sceneFilePath);
				m_engine.setNumRenderThreads(Integer.parseInt(getRenderSetting().getNumThreads().getValue()));
				m_engine.setWorkingDirectory(sceneFile.getParent());
				m_engine.loadCommand(sceneFilePath);
				
				m_engine.update();
				
				FilmInfo info = m_engine.getFilmInfo();
				if(info.widthPx  != m_finalFrame.widthPx() || 
				   info.heightPx != m_finalFrame.heightPx())
				{
					m_finalFrame.dispose();
					m_transientFrame.dispose();
					m_finalFrame = new PhFrame(info.widthPx, info.heightPx);
					m_transientFrame = new PhFrame(info.widthPx, info.heightPx);
				}
				
				return null;
			}
		};
	}
	
	private Task<Void> newUpdateStaticImageTask()
	{
		return new Task<Void>()
		{
			@Override
			protected Void call() throws Exception
			{
				Platform.runLater(() -> 
					getLogView().showLog("developing film..."));
			
				m_engine.developFilm(m_finalFrame);
				m_finalFrame.getFullRgb(m_localFinalFrame);
				
				return null;
			}
		};
	}
	
	private Runnable newRenderStatusQuery()
	{
		ObservableRenderData data = m_engine.getObservableRenderData();
		
		List<RenderStateEntry> states = new ArrayList<>();
		for(int i = 0; i < data.integerNames.length; ++i)
		{
			RenderStateEntry integerState = RenderStateEntry.newInteger(data.integerNames[i], i);
			if(!integerState.getName().isEmpty())
			{
				states.add(integerState);
			}
		}
		for(int i = 0; i < data.realNames.length; ++i)
		{
			RenderStateEntry realState = RenderStateEntry.newReal(data.realNames[i], i);
			if(!realState.getName().isEmpty())
			{
				states.add(realState);
			}
		}
		
		return new RenderStatusQuery(this, states, m_renderStatusView);
	}
}

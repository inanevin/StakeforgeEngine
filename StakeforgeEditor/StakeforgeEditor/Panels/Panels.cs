using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StakeforgeEditor.Panels
{

	class Panels
	{
		private Main.TopBarViewModel _topBarViewModel = new Main.TopBarViewModel();

		private PropertiesViewModel _propertiesViewModel = new PropertiesViewModel();
		private ConsoleViewModel _consoleViewModel = new ConsoleViewModel();
		private ResourcesViewModel _resourcesViewModel = new ResourcesViewModel();
		private EntitiesViewModel _entitiesViewModel = new EntitiesViewModel();
		private WorldViewModel _worldViewModel = new WorldViewModel();
		private static Panels? _instance = null;

		public PropertiesViewModel PropertiesViewModel { get => _propertiesViewModel; }
		public ConsoleViewModel ConsoleViewModel { get => _consoleViewModel; }
		public ResourcesViewModel ResourcesViewModel { get => _resourcesViewModel; }
		public EntitiesViewModel EntitiesViewModel { get => _entitiesViewModel; }
		public WorldViewModel WorldViewModel { get => _worldViewModel; }

		public Main.TopBarViewModel TopBarViewModel { get => _topBarViewModel; }

		public static Panels Instance
		{
			get
			{
				if (_instance == null)
					_instance = new Panels();
				return _instance;
			}
		}

		public PanelViewModel GetPanelByType(Common.PanelType type)
		{
			switch (type)
			{
				case Common.PanelType.Resources:
					return _resourcesViewModel;
				case Common.PanelType.Properties:
					return _propertiesViewModel;
				case Common.PanelType.Entities:
					return _entitiesViewModel;
				case Common.PanelType.Console:
					return _consoleViewModel;
				case Common.PanelType.World:
					return _worldViewModel;
				default:
					return _resourcesViewModel;
			}
		}
	}
}

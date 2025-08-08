using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace StakeforgeEditor.Panels
{
	class Panels
	{
		private ConsoleViewModel _consoleViewModel = new ConsoleViewModel();
		private ResourcesViewModel _resourcesViewModel = new ResourcesViewModel();
		private EntitiesViewModel _entitiesViewModel = new EntitiesViewModel();
		private static Panels? _instance = null;

		public ConsoleViewModel ConsoleViewModel { get => _consoleViewModel; }
		public ResourcesViewModel ResourcesViewModel { get => _resourcesViewModel; }
		public EntitiesViewModel EntitiesViewModel { get => _entitiesViewModel; }

		public static Panels Instance
		{
			get
			{
				if (_instance == null)
					_instance = new Panels();
				return _instance;
			}
		}
	}
}

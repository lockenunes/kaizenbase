#include <vector>
#include "global.h"
#include "smile.h"

namespace KaizenLibrary
{

	class SampleMixer;
	/**
		Tipos de express�es ponderadas utilizadas.
	*/
	enum WExpression
	{
		WMEAN, WMIN, WMAX, MIXMINMAX, NONE
	};

	/**
		Representa��o de um n� ranqueado.
	*/
	class RNode
	{
	public:
		// Incrementa em +1 sempre que um n� ranqueado � criado.
		// Representa o identificador �nico do n� ranqueado. 
		// Preciso dessa refer�ncia para reconstruir as rela��es entre os n�s ao carregar uma rede bayesiana salva.
		static int current_id;  

		RNode();				// Construtor padr�o.
		RNode(int id);			// Construtor utilizado quando a rede bayesiana � carregada de um arquivo salvo.
		~RNode();

		// Configura o n� ranqueado de acordo com o n�mero de estados.
		// @deprecated
		void Init3StateNode(std::string state1, std::string state2, std::string state3, double inVar = DEFAULT_VAR, double inMu = DEFAULT_MU);
	
		// Cada n� ranquado � convertido para o formato smile para manipula��o utilizando o adaptador.
		// Handler �til para trocar informa��es entre as duas inst�ncias.
		int smileHandler;

		// Express�o ponderada utilizada na mistura das amostras.
		WExpression wExpression;
	
		/**
			Inicializa o n� ranqueado configurando os 
			atributos que forem necess�rios.
		*/
		void init();

		/**
			Adiciona um n�-pai e seu peso em um n�-filho.
	 
			@param parentNode n�-pai.
			@param parentWeight influ�ncia do n� pai exercido sobre o n�-filho (peso).
		*/
		void addParent(RNode* parentNode, double parentWeight);
	
		/**
			Adiciona um ponteiro de um n�-filho para um n�-pai.
	 
			@param childNode ponteiro que aponta para o n�-filho.
		*/
		void setChild(RNode* childNode);

		// Como resolver essa parada?
		void setState(int pos, std::string name);

		void setStateValue(int pos, double value);

		/**
			Navegabilidade entre n�s-pai e n�s-filho.
			Um n�-pai tem acesso aos seus n�s-filhos e um n�-filho tem acesso aos seus n�s-pais. 
			Utiliza addParent(...) e setChild(...) para configurar as refer�ncias em ambos os n�s (i.e., pais e filhos).
	 
			@param parentNode n�-pai.
		*/
		void setParent(RNode* parentNode);
	
		/**
			Configura o peso de um n�-pai. 
			O peso deve ser guardado em um vetor e est� no mesmo �ndice
			do n�-pai a que se refere (no vetor de n�s-pais).
	 
			@param parentWeight peso do n�-pai.
		*/
		void setParentWeight(double parentWeight);
	
		/**
			Gera a tabela de probabilidade de n�s baseado nos n�s-pais.
			#Depreciado

			@param parentIndex �ndice do n�-pai que est� sendo processado.
		*/
		void vertigo(int parentIndex = 0); 
	
		/**
			Remove um n�-pai.
			Deve remover a refer�ncia para este n�-filho no n�-pai 
			que est� sendo removido.

			@param parentNode n�-pai que ser� removido.
		*/
		void removeParent(RNode* parentNode);

		/**
			Apenas remove o n�-pai (i.e., n�o se importa em remover a 
			refer�ncia que aponta para o n�-filho presente no n�-pai
			que ser� removido.

			@param parentNode n�-pai que ser� removido.
		*/
		void removeParentOnly(RNode* parentNode);

		/**
			Remove um n�-filho.

			@param childNode n�-filho que ser� removido.
		*/
		void removeChild(RNode* childNode);

		/**
			Reseta os valores dos estados do n� ranqueado.
		*/
		void resetStateValues();

		/**
			Preenche um arquivo DSL_sysCoordinates pr�prio do smile com
			valores da tabela de probabilidade do n� ranqueado.
		
			@param coordinates arquivo DSL_sysCoordinates que ser� preenchido.
		*/
		void fillSmileFileCPTValues(DSL_sysCoordinates *coordinates);

		// Setters

		/**
			Configura a mediana do n� ranqueado.

			@param inMu mediana de entrada do n� ranqueado.
		*/
		void setMean(double inMu);

		/**
			Congigura a vari�ncia do n� ranqueado.
	 
			@param inVar vari�ncia de entrada do n� ranqueado.
		*/
		void setVariance(double inVar);

		/**
			M�todo set para a tabela de probabilidade dos n�s.

			@param tpn vetor que representa a tabela de probabilidade dos n�s.
		*/
		void setTpn(std::vector<double> tpn);

		/**
			M�todo set para as amostras do n� ranqueado.

			@param samples vetor de amostras.
		*/
		void setSamples(std::vector<double> samples);
	
		/** 
			Configura o nome do n� ranqueado.
		*/
		void setName(std::string name);
	
		/**
			Configura a evid�ncia ou observa��o do n�. 
			Digamos que tenhamos um n� ranqueado com 5 estados,
			o menor valor de evid�ncia deve ser 0 e o m�ximo deve
			ser 4.	
	 
			@param stateEvidence short que representa a evidencia.
		*/
		void setEvidence(short stateEvidence);

		/**
			Configura��o de parametros da express�o mixminmax.
			Configura o valor m�nimo.

			@param wminValue valor m�nimo da express�o mixminmax.
		*/
		void setWMinValue(double wminValue = 1);

		/**
			Configura��o de parametros da express�o mixminmax.
			Configura o valor m�ximo.

			@param wmaxValue valor m�ximo da express�o mixminmax.
		*/
		void setWMaxValue(double wmaxValue = 1);

		// Getters
	
		/**
			Retorna true caso tenha pelo menos um n�-pai.
		*/
		bool isChild();
	
		/**
			Recupera n�s-pais vinculados a este n�.
	 
			@return ponteiro para o vetor de n�s-pais.
		*/
		std::vector<RNode*>* getParentsPtr();

		/**
			Recupera n�s-filhos vinculados a este n�.

			@return ponteiro para vetor de n�s-filhos.
		*/
		std::vector<RNode*>* getChildsPtr();

		/**
			Recupera tabela de probabilidade dos n�s.

			@return ponteiro para o vetor que armazena os valores da tpn.
		*/
		std::vector<double>* getTpnPtr();

		/**
			Recupera nomes dos estados do n�.

			@return ponteiro para vetor que armazena os nomes dos estaods do n�.
		*/
		std::vector<std::string>* getStateTitlesPtr();
	
		/**
			Recupera valores dos estados do n�.

			@return ponteiro para vetor de valores dos estados do n�.
		*/
		std::vector<double>* getStateValuesPtr();

		/**
			Recupera intervalos de estados dos n�s.

			@return ponteiro para vetor de intervalos de estados dos n�s.
		*/
		std::vector<double>* getStateIntervalsPtr();

		/**
			Recupera as amostras do n� ranqueado.

			@return ponteiro para vetor de amostras do n� ranqueado.
		*/
		std::vector<double>* getSamplesPtr();

		/**
			Recupera o peso dos n�s-pais.

			@return ponteiro para vetor que armazena os pesos dos n�s-pais.
		*/
		std::vector<double>* getParentsWeightPtr();

		/**
			Recupera os n�s-pais deste n�.

			@return vetor que cont�m refer�ncias para n�s-pais.
		*/
		std::vector<RNode*> getParents();

		/** 
			Recupera o identificador dos n�s ranqueados.
			Este identificador � importante quando for gravar e
			recuperar o arquivo json que guarda a rede bayesiana
			e todas as configura��es dos n�s ranqueados.

			@return vetor contendo id dos n�s-pais.
		*/
		std::vector<int> getParentsId();
	
		/**
			Recupera o n�mero de estados do n� ranquedo.

			@return inteiro que representa o n�mero de estaods. TODO substituir por short
		*/
		int getNumberOfStates();

		/**
			Recupera vari�ncia do n� ranqueado.

			@return double representando vari�ncia do n� ranqueado.
		*/
		double getVariance();

		/**
			Recupera o tamnho do n� ranqueado.
			Por tamanho, considera-se a quantidade de n�s-pais do n�.
	
			@return inteiro que representada o tamanho do n� ranqueado.
		*/
		int size();

		/**
			Recupera o nome do n� ranqueado.
	 
			@return String com nome do n� ranqueado.
			TODO Jo�o
		*/
		std::string getName();

		/**
			Recupera a evid�ncia/observa��o do n� ranqueado.

			@return short que representa o estado do n� em evid�ncia.
		*/
		short getEvidence();

		/**
			Recupera o identificador do n� ranqueado.

			@return int que representa o identificador do n�.
		*/
		int getId();

		/**
			Mistura as amostras dos n�s-pais utilizando a express�o de m�dia ponderada (WMEAN).
		*/
		void weigthedMean();

		/**
			Mistura as amostras dos n�s-pais utilizando a express�o de Weighted Min.
		*/
		double weigthedMin();

		/**
			Mistura as amostras dos n�s-pais utilizando a express�o de Weighted Max.
		*/
		double weigthedMax();

		/**
			Mistura as amostras dos n�s-pais utilizando a express�o de MixMinMax.
		*/
		void mixMinMax();
	
		// This is temporary, just trying to get to old solution's perfomance 
		// TODO Ainda preciso desse m�todo?
		void fillSmileCPT(DSL_sysCoordinates *coordinates, int parentIndex);

		/**
			Gera amostras dado uma m�dia de entrada.

			@param inMu m�dia da distribui��o.
		*/
		void generateEvidence(double inMu);
	
		// Util
		/**
			Imprime os estados dos n�s para fins de debug.
		*/
		void print();

		/**
			Registra o identificador de um n�-pai. 
			O identificador � usado para auxiliar na cria��o 
			das rela��es quando do carregamento dos dados de 
			um arquivo.

			@param id identificador �nico do n�-ranqueado.
		*/
		void addParentId(int id);

		/**
			Registra o identificador de um n�-filho.
			O identificador � usado para auxiliar na cria��o 
			das rela��es quando do carregamento dos dados de 
			um arquivo.

			@param id identificador �nico do n�-ranqueado
		*/
		void addChildId(int id);

		/**
			Recupera o identificador de um n�-pai.

			@param parentIndex posi��o do n� no vetor de n�s-pais.
		*/
		int getParentId(int parentIndex);

		/**
			Recupera o identificador de um n�-filho.

			@param childIndex posi��o do n� no vetor de n�s-filhos.
		*/
		int getChildId(int childIndex);

		/**
			Recupera valor m�nimo para uso na express�o MixMinMax.
		*/
		double getWMinValue();
	
		/**
			Recupera valor m�ximo para uso na express�o MixMinMax.
		*/
		double getWMaxValue();

		/**
			Preenche uma tabela de probabilidade dos n�s vazia
			com "0"s.

			@parentIndex �ndice do n�-pai no vetor de n�s-pais.
		*/
		void fillEmptyNPT(int parentIndex = 0);

		/**
			Notifica quem for necess�rio quando o n�mero de estados
			do n�-ranqueado for alterado.
		*/
		void numberOfStatesChanged();

		/**
			Configura a necessidade de atualizar ou n�o a tabela de 
			probabilidades dos n�s.
		*/
		void setNptNeedUpdate(bool nptOutdated);

		bool manualNptNeedUpdate();

		/**
			Notifica os n�s-filhos quando este n�-ranqueado for 
			deletado.
		*/
		void notifyChildOnDelete();

		// Talvez deva implementar um builder para construir os n�s. TODO Ver depois.
		/**
			Adiciona um novo estado ao n� ranqueado.
	 
			@param name nome que representa o estado do n� ranqueado.	
		*/
		RNode *withState(std::string name);

		/**
			Atribui uma vari�ncia ao n� ranqueado.
	 
			@param inVar vari�ncia de entrada.
		*/
		RNode* withVariance(double inVar);

		/**
			Atribui uma m�dia para o n� ranqueado.
	 
			@param inMu m�dia de entrada.
		*/
		RNode* withMean(double inMu);
	
		/**
			Atribui um nome ao n� ranqueado.
	 
			@param name nome do n� ranqueado.
		*/
		RNode* withName(std::string name);

		void setId(int id);

		void setSampleMixer(SampleMixer *mixer);

		void setSampleMixer();

		void wmean();

		void cleanStateValues();

		void notifyChanges();

		void setFunction(std::string function);

		void addNPTEntry(double entry);

	private:
		std::vector<double> _samples;			// Vetor de amostras.
		std::vector<double> _tpn;				// Tabela de probabilidade dos n�s.
		std::vector<double> _parentsWeight;		// Vetor que armazena a influ�ncia dos n�s-pais (peso).
		std::vector<RNode*> _parents;			// Vetor que armazena refer�ncia para os n�s-pais.
		std::vector<int> _parentsId;			// Vetor que armazena o identificador dos n�s-pais.
		std::vector<RNode*> _childNodes;		// Vetor que armazena os n�s-filhos.  
		std::vector<int> _childrenId;		    // Vetor que armazena o identificador dos n�s-filhos.
		std::vector<std::string> _stateTitles;	// Vetor que armazena o nome dos estados do n�-ranqueado.
		std::vector<double> _stateValues;       // Vetor que armazena os valores dos estados do n�-ranqueado.
		std::vector<double> _stateIntervals;    // Vetor que armazena os valor dos intervalos dos estados do n�.
		std::string _name;						// Nome do n�-ranqueado.
		double _mu;								// M�dia do n�-raqueado.
		double _var;							// Vari�ncia do n�-ranqueado.
		int _nStates;							// Quantidade de estados do n� ranqueado.
		int _evidence;							// Evid�ncia do n�-ranqueado (qual estado foi selecionado na obs)
		int _id;								// Identificador �nico deste n�-ranqueado.
		double _wminValue;						// Valor m�nimo utilizado na express�o MixMinMax.
		double _wmaxValue;						// Valor m�ximo utilizado na express�o MixMinMax.
		bool _manualNptneedUpdate;				// Indica a necessidade ou n�o de atualiza��o da tabela de probabilidade.
		SampleMixer *_sampleMixer;

	};
}
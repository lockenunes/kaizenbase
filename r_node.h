#include <vector>
#include "global.h"
#include "smile.h"

namespace KaizenLibrary
{

	class SampleMixer;
	/**
		Tipos de expressões ponderadas utilizadas.
	*/
	enum WExpression
	{
		WMEAN, WMIN, WMAX, MIXMINMAX, NONE
	};

	/**
		Representação de um nó ranqueado.
	*/
	class RNode
	{
	public:
		// Incrementa em +1 sempre que um nó ranqueado é criado.
		// Representa o identificador único do nó ranqueado. 
		// Preciso dessa referência para reconstruir as relações entre os nós ao carregar uma rede bayesiana salva.
		static int current_id;  

		RNode();				// Construtor padrão.
		RNode(int id);			// Construtor utilizado quando a rede bayesiana é carregada de um arquivo salvo.
		~RNode();

		// Configura o nó ranqueado de acordo com o número de estados.
		// @deprecated
		void Init3StateNode(std::string state1, std::string state2, std::string state3, double inVar = DEFAULT_VAR, double inMu = DEFAULT_MU);
	
		// Cada nó ranquado é convertido para o formato smile para manipulação utilizando o adaptador.
		// Handler útil para trocar informações entre as duas instâncias.
		int smileHandler;

		// Expressão ponderada utilizada na mistura das amostras.
		WExpression wExpression;
	
		/**
			Inicializa o nó ranqueado configurando os 
			atributos que forem necessários.
		*/
		void init();

		/**
			Adiciona um nó-pai e seu peso em um nó-filho.
	 
			@param parentNode nó-pai.
			@param parentWeight influência do nó pai exercido sobre o nó-filho (peso).
		*/
		void addParent(RNode* parentNode, double parentWeight);
	
		/**
			Adiciona um ponteiro de um nó-filho para um nó-pai.
	 
			@param childNode ponteiro que aponta para o nó-filho.
		*/
		void setChild(RNode* childNode);

		// Como resolver essa parada?
		void setState(int pos, std::string name);

		void setStateValue(int pos, double value);

		/**
			Navegabilidade entre nós-pai e nós-filho.
			Um nó-pai tem acesso aos seus nós-filhos e um nó-filho tem acesso aos seus nós-pais. 
			Utiliza addParent(...) e setChild(...) para configurar as referências em ambos os nós (i.e., pais e filhos).
	 
			@param parentNode nó-pai.
		*/
		void setParent(RNode* parentNode);
	
		/**
			Configura o peso de um nó-pai. 
			O peso deve ser guardado em um vetor e está no mesmo índice
			do nó-pai a que se refere (no vetor de nós-pais).
	 
			@param parentWeight peso do nó-pai.
		*/
		void setParentWeight(double parentWeight);
	
		/**
			Gera a tabela de probabilidade de nós baseado nos nós-pais.
			#Depreciado

			@param parentIndex índice do nó-pai que está sendo processado.
		*/
		void vertigo(int parentIndex = 0); 
	
		/**
			Remove um nó-pai.
			Deve remover a referência para este nó-filho no nó-pai 
			que está sendo removido.

			@param parentNode nó-pai que será removido.
		*/
		void removeParent(RNode* parentNode);

		/**
			Apenas remove o nó-pai (i.e., não se importa em remover a 
			referência que aponta para o nó-filho presente no nó-pai
			que será removido.

			@param parentNode nó-pai que será removido.
		*/
		void removeParentOnly(RNode* parentNode);

		/**
			Remove um nó-filho.

			@param childNode nó-filho que será removido.
		*/
		void removeChild(RNode* childNode);

		/**
			Reseta os valores dos estados do nó ranqueado.
		*/
		void resetStateValues();

		/**
			Preenche um arquivo DSL_sysCoordinates próprio do smile com
			valores da tabela de probabilidade do nó ranqueado.
		
			@param coordinates arquivo DSL_sysCoordinates que será preenchido.
		*/
		void fillSmileFileCPTValues(DSL_sysCoordinates *coordinates);

		// Setters

		/**
			Configura a mediana do nó ranqueado.

			@param inMu mediana de entrada do nó ranqueado.
		*/
		void setMean(double inMu);

		/**
			Congigura a variância do nó ranqueado.
	 
			@param inVar variância de entrada do nó ranqueado.
		*/
		void setVariance(double inVar);

		/**
			Método set para a tabela de probabilidade dos nós.

			@param tpn vetor que representa a tabela de probabilidade dos nós.
		*/
		void setTpn(std::vector<double> tpn);

		/**
			Método set para as amostras do nó ranqueado.

			@param samples vetor de amostras.
		*/
		void setSamples(std::vector<double> samples);
	
		/** 
			Configura o nome do nó ranqueado.
		*/
		void setName(std::string name);
	
		/**
			Configura a evidência ou observação do nó. 
			Digamos que tenhamos um nó ranqueado com 5 estados,
			o menor valor de evidência deve ser 0 e o máximo deve
			ser 4.	
	 
			@param stateEvidence short que representa a evidencia.
		*/
		void setEvidence(short stateEvidence);

		/**
			Configuração de parametros da expressão mixminmax.
			Configura o valor mínimo.

			@param wminValue valor mínimo da expressão mixminmax.
		*/
		void setWMinValue(double wminValue = 1);

		/**
			Configuração de parametros da expressão mixminmax.
			Configura o valor máximo.

			@param wmaxValue valor máximo da expressão mixminmax.
		*/
		void setWMaxValue(double wmaxValue = 1);

		// Getters
	
		/**
			Retorna true caso tenha pelo menos um nó-pai.
		*/
		bool isChild();
	
		/**
			Recupera nós-pais vinculados a este nó.
	 
			@return ponteiro para o vetor de nós-pais.
		*/
		std::vector<RNode*>* getParentsPtr();

		/**
			Recupera nós-filhos vinculados a este nó.

			@return ponteiro para vetor de nós-filhos.
		*/
		std::vector<RNode*>* getChildsPtr();

		/**
			Recupera tabela de probabilidade dos nós.

			@return ponteiro para o vetor que armazena os valores da tpn.
		*/
		std::vector<double>* getTpnPtr();

		/**
			Recupera nomes dos estados do nó.

			@return ponteiro para vetor que armazena os nomes dos estaods do nó.
		*/
		std::vector<std::string>* getStateTitlesPtr();
	
		/**
			Recupera valores dos estados do nó.

			@return ponteiro para vetor de valores dos estados do nó.
		*/
		std::vector<double>* getStateValuesPtr();

		/**
			Recupera intervalos de estados dos nós.

			@return ponteiro para vetor de intervalos de estados dos nós.
		*/
		std::vector<double>* getStateIntervalsPtr();

		/**
			Recupera as amostras do nó ranqueado.

			@return ponteiro para vetor de amostras do nó ranqueado.
		*/
		std::vector<double>* getSamplesPtr();

		/**
			Recupera o peso dos nós-pais.

			@return ponteiro para vetor que armazena os pesos dos nós-pais.
		*/
		std::vector<double>* getParentsWeightPtr();

		/**
			Recupera os nós-pais deste nó.

			@return vetor que contém referências para nós-pais.
		*/
		std::vector<RNode*> getParents();

		/** 
			Recupera o identificador dos nós ranqueados.
			Este identificador é importante quando for gravar e
			recuperar o arquivo json que guarda a rede bayesiana
			e todas as configurações dos nós ranqueados.

			@return vetor contendo id dos nós-pais.
		*/
		std::vector<int> getParentsId();
	
		/**
			Recupera o número de estados do nó ranquedo.

			@return inteiro que representa o número de estaods. TODO substituir por short
		*/
		int getNumberOfStates();

		/**
			Recupera variância do nó ranqueado.

			@return double representando variância do nó ranqueado.
		*/
		double getVariance();

		/**
			Recupera o tamnho do nó ranqueado.
			Por tamanho, considera-se a quantidade de nós-pais do nó.
	
			@return inteiro que representada o tamanho do nó ranqueado.
		*/
		int size();

		/**
			Recupera o nome do nó ranqueado.
	 
			@return String com nome do nó ranqueado.
			TODO João
		*/
		std::string getName();

		/**
			Recupera a evidência/observação do nó ranqueado.

			@return short que representa o estado do nó em evidência.
		*/
		short getEvidence();

		/**
			Recupera o identificador do nó ranqueado.

			@return int que representa o identificador do nó.
		*/
		int getId();

		/**
			Mistura as amostras dos nós-pais utilizando a expressão de média ponderada (WMEAN).
		*/
		void weigthedMean();

		/**
			Mistura as amostras dos nós-pais utilizando a expressão de Weighted Min.
		*/
		double weigthedMin();

		/**
			Mistura as amostras dos nós-pais utilizando a expressão de Weighted Max.
		*/
		double weigthedMax();

		/**
			Mistura as amostras dos nós-pais utilizando a expressão de MixMinMax.
		*/
		void mixMinMax();
	
		// This is temporary, just trying to get to old solution's perfomance 
		// TODO Ainda preciso desse método?
		void fillSmileCPT(DSL_sysCoordinates *coordinates, int parentIndex);

		/**
			Gera amostras dado uma média de entrada.

			@param inMu média da distribuição.
		*/
		void generateEvidence(double inMu);
	
		// Util
		/**
			Imprime os estados dos nós para fins de debug.
		*/
		void print();

		/**
			Registra o identificador de um nó-pai. 
			O identificador é usado para auxiliar na criação 
			das relações quando do carregamento dos dados de 
			um arquivo.

			@param id identificador único do nó-ranqueado.
		*/
		void addParentId(int id);

		/**
			Registra o identificador de um nó-filho.
			O identificador é usado para auxiliar na criação 
			das relações quando do carregamento dos dados de 
			um arquivo.

			@param id identificador único do nó-ranqueado
		*/
		void addChildId(int id);

		/**
			Recupera o identificador de um nó-pai.

			@param parentIndex posição do nó no vetor de nós-pais.
		*/
		int getParentId(int parentIndex);

		/**
			Recupera o identificador de um nó-filho.

			@param childIndex posição do nó no vetor de nós-filhos.
		*/
		int getChildId(int childIndex);

		/**
			Recupera valor mínimo para uso na expressão MixMinMax.
		*/
		double getWMinValue();
	
		/**
			Recupera valor máximo para uso na expressão MixMinMax.
		*/
		double getWMaxValue();

		/**
			Preenche uma tabela de probabilidade dos nós vazia
			com "0"s.

			@parentIndex índice do nó-pai no vetor de nós-pais.
		*/
		void fillEmptyNPT(int parentIndex = 0);

		/**
			Notifica quem for necessário quando o número de estados
			do nó-ranqueado for alterado.
		*/
		void numberOfStatesChanged();

		/**
			Configura a necessidade de atualizar ou não a tabela de 
			probabilidades dos nós.
		*/
		void setNptNeedUpdate(bool nptOutdated);

		bool manualNptNeedUpdate();

		/**
			Notifica os nós-filhos quando este nó-ranqueado for 
			deletado.
		*/
		void notifyChildOnDelete();

		// Talvez deva implementar um builder para construir os nós. TODO Ver depois.
		/**
			Adiciona um novo estado ao nó ranqueado.
	 
			@param name nome que representa o estado do nó ranqueado.	
		*/
		RNode *withState(std::string name);

		/**
			Atribui uma variância ao nó ranqueado.
	 
			@param inVar variância de entrada.
		*/
		RNode* withVariance(double inVar);

		/**
			Atribui uma média para o nó ranqueado.
	 
			@param inMu média de entrada.
		*/
		RNode* withMean(double inMu);
	
		/**
			Atribui um nome ao nó ranqueado.
	 
			@param name nome do nó ranqueado.
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
		std::vector<double> _tpn;				// Tabela de probabilidade dos nós.
		std::vector<double> _parentsWeight;		// Vetor que armazena a influência dos nós-pais (peso).
		std::vector<RNode*> _parents;			// Vetor que armazena referência para os nós-pais.
		std::vector<int> _parentsId;			// Vetor que armazena o identificador dos nós-pais.
		std::vector<RNode*> _childNodes;		// Vetor que armazena os nós-filhos.  
		std::vector<int> _childrenId;		    // Vetor que armazena o identificador dos nós-filhos.
		std::vector<std::string> _stateTitles;	// Vetor que armazena o nome dos estados do nó-ranqueado.
		std::vector<double> _stateValues;       // Vetor que armazena os valores dos estados do nó-ranqueado.
		std::vector<double> _stateIntervals;    // Vetor que armazena os valor dos intervalos dos estados do nó.
		std::string _name;						// Nome do nó-ranqueado.
		double _mu;								// Média do nó-raqueado.
		double _var;							// Variância do nó-ranqueado.
		int _nStates;							// Quantidade de estados do nó ranqueado.
		int _evidence;							// Evidência do nó-ranqueado (qual estado foi selecionado na obs)
		int _id;								// Identificador único deste nó-ranqueado.
		double _wminValue;						// Valor mínimo utilizado na expressão MixMinMax.
		double _wmaxValue;						// Valor máximo utilizado na expressão MixMinMax.
		bool _manualNptneedUpdate;				// Indica a necessidade ou não de atualização da tabela de probabilidade.
		SampleMixer *_sampleMixer;

	};
}
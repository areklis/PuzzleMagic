#include "PuzzleRiddles.h"

// Meant for a casual game: mostly things a player half-remembers from films, books and Halloween nights, with a few
// harder ones for variety. Keep to well-documented facts and widely told legends; the right answer goes first.
const TArray<FPuzzleRiddle>& PuzzleRiddles()
{
	static const TArray<FPuzzleRiddle> List = {
		// --- Halloween ---
		{ TEXT("HALLOWEEN"), TEXT("What do people carve faces into for Halloween?"),
		  { TEXT("Pumpkins"), TEXT("Watermelons"), TEXT("Coconuts"), TEXT("Pineapples") },
		  TEXT("Lit with a candle inside, a carved pumpkin is called a jack-o'-lantern.") },
		{ TEXT("HALLOWEEN"), TEXT("What do children say at the door on Halloween?"),
		  { TEXT("Trick or treat!"), TEXT("Happy holidays!"), TEXT("Open sesame!"), TEXT("Knock knock!") },
		  TEXT("A treat keeps the trick away... or so the bargain goes.") },
		{ TEXT("HALLOWEEN"), TEXT("What does a witch traditionally fly on?"),
		  { TEXT("A broomstick"), TEXT("A carpet"), TEXT("A cloud"), TEXT("A dragon") },
		  TEXT("Old woodcuts show witches riding brooms, pitchforks and even goats to their midnight gatherings.") },
		{ TEXT("HALLOWEEN"), TEXT("On which date is Halloween?"),
		  { TEXT("October 31"), TEXT("November 13"), TEXT("October 1"), TEXT("December 31") },
		  TEXT("The eve of All Saints' Day, November 1.") },
		{ TEXT("HALLOWEEN"), TEXT("What are the classic colours of Halloween?"),
		  { TEXT("Orange and black"), TEXT("Red and green"), TEXT("Blue and white"), TEXT("Pink and gold") },
		  TEXT("Orange for the harvest and the pumpkin, black for the long night.") },
		{ TEXT("HALLOWEEN"), TEXT("A carved pumpkin with a candle inside is called a what?"),
		  { TEXT("Jack-o'-lantern"), TEXT("Will-o'-lamp"), TEXT("Ghost light"), TEXT("Pumpkin torch") },
		  TEXT("Named after Stingy Jack, who tricked the Devil and was left to wander with only a lantern.") },
		{ TEXT("HALLOWEEN"), TEXT("A witch brews her potions in a what?"),
		  { TEXT("Cauldron"), TEXT("Teapot"), TEXT("Barrel"), TEXT("Bathtub") },
		  TEXT("\"Double, double toil and trouble; fire burn and cauldron bubble\": Shakespeare's witches in Macbeth.") },
		{ TEXT("HALLOWEEN"), TEXT("Which animal crossing your path is said to bring bad luck?"),
		  { TEXT("A black cat"), TEXT("A white rabbit"), TEXT("A brown dog"), TEXT("A grey mouse") },
		  TEXT("In medieval Europe black cats were thought to be witches' companions, or witches in disguise.") },
		{ TEXT("HALLOWEEN"), TEXT("What does a ghost famously say to scare you?"),
		  { TEXT("Boo!"), TEXT("Moo!"), TEXT("Shoo!"), TEXT("Woof!") },
		  TEXT("Short, sudden and loud: the oldest trick in the haunted book.") },
		{ TEXT("HALLOWEEN"), TEXT("Which flying animal hangs upside down and is a Halloween symbol?"),
		  { TEXT("The bat"), TEXT("The owl"), TEXT("The crow"), TEXT("The moth") },
		  TEXT("Bats came out over the old Samhain bonfires, drawn by the insects in the firelight.") },
		{ TEXT("HALLOWEEN"), TEXT("Which fruit do you try to catch with your teeth in a tub of water?"),
		  { TEXT("Apples"), TEXT("Grapes"), TEXT("Bananas"), TEXT("Oranges") },
		  TEXT("Bobbing for apples was once a fortune game: the first to bite one would be the first to marry.") },
		{ TEXT("HALLOWEEN"), TEXT("What are the three colours of candy corn?"),
		  { TEXT("Yellow, orange and white"), TEXT("Red, white and blue"), TEXT("Black, orange and purple"), TEXT("Green, yellow and red") },
		  TEXT("The stripes copy a kernel of corn, from the days when America was mostly farmland.") },
		{ TEXT("HALLOWEEN"), TEXT("A house where ghosts are said to live is called what?"),
		  { TEXT("Haunted"), TEXT("Hollow"), TEXT("Hallowed"), TEXT("Hidden") },
		  TEXT("Creaking stairs, cold spots and doors that close by themselves: the usual signs.") },
		{ TEXT("HALLOWEEN"), TEXT("What shape is a witch's hat?"),
		  { TEXT("Pointed"), TEXT("Flat"), TEXT("Round"), TEXT("Square") },
		  TEXT("The tall pointed hat became the witch's mark in storybooks and later in films.") },
		{ TEXT("HALLOWEEN"), TEXT("Halloween is short for what?"),
		  { TEXT("All Hallows' Eve"), TEXT("Hallowed Moon"), TEXT("Hall of Ghosts"), TEXT("Hollow Evening") },
		  TEXT("\"Hallow\" is an old word for a saint: Halloween is the eve of All Saints' Day.") },
		{ TEXT("HALLOWEEN"), TEXT("Before pumpkins, what did the Irish carve jack-o'-lanterns from?"),
		  { TEXT("Turnips"), TEXT("Potatoes"), TEXT("Apples"), TEXT("Onions") },
		  TEXT("Irish emigrants to America found pumpkins bigger and easier to carve, and the pumpkin took over.") },
		{ TEXT("HALLOWEEN"), TEXT("Halloween grew out of which old Celtic festival?"),
		  { TEXT("Samhain"), TEXT("Beltane"), TEXT("Yule"), TEXT("Imbolc") },
		  TEXT("Samhain marked the end of summer, when the wall between the living and the dead was thought thinnest.") },
		{ TEXT("HALLOWEEN"), TEXT("Which famous escape artist died on Halloween, 1926?"),
		  { TEXT("Harry Houdini"), TEXT("David Copperfield"), TEXT("Harry Potter"), TEXT("Merlin") },
		  TEXT("For years his widow held a séance every Halloween, waiting for a message that never came.") },
		{ TEXT("HALLOWEEN"), TEXT("Is a pumpkin a fruit or a vegetable?"),
		  { TEXT("A fruit"), TEXT("A vegetable"), TEXT("A root"), TEXT("A nut") },
		  TEXT("It grows from the flower and holds the seeds, so botanists call it a fruit.") },

		// --- Monsters ---
		{ TEXT("MONSTERS"), TEXT("Which monster drinks blood and fears garlic?"),
		  { TEXT("A vampire"), TEXT("A werewolf"), TEXT("A zombie"), TEXT("A mummy") },
		  TEXT("Garlic on the windows and doors was once a real precaution in Eastern European villages.") },
		{ TEXT("MONSTERS"), TEXT("What turns into a wolf under the full moon?"),
		  { TEXT("A werewolf"), TEXT("A vampire"), TEXT("A ghost"), TEXT("A goblin") },
		  TEXT("\"Were\" is an old English word for man: a werewolf is a man-wolf.") },
		{ TEXT("MONSTERS"), TEXT("In films, what kind of bullet stops a werewolf?"),
		  { TEXT("Silver"), TEXT("Gold"), TEXT("Iron"), TEXT("Wooden") },
		  TEXT("The silver bullet comes mostly from 19th-century tales and Hollywood.") },
		{ TEXT("MONSTERS"), TEXT("What kind of creature is Count Dracula?"),
		  { TEXT("A vampire"), TEXT("A werewolf"), TEXT("A ghost"), TEXT("A wizard") },
		  TEXT("Bram Stoker's count made the vampire famous all over the world.") },
		{ TEXT("MONSTERS"), TEXT("Where is Count Dracula's castle?"),
		  { TEXT("Transylvania"), TEXT("Scotland"), TEXT("Egypt"), TEXT("Iceland") },
		  TEXT("A mountain region of Romania, ringed by the Carpathians.") },
		{ TEXT("MONSTERS"), TEXT("Where does a vampire sleep during the day?"),
		  { TEXT("In a coffin"), TEXT("In a hammock"), TEXT("In a tree"), TEXT("Under the bed") },
		  TEXT("Sunlight is deadly to most vampires of legend and film, so they rest in the dark.") },
		{ TEXT("MONSTERS"), TEXT("What can't you see when a vampire stands before a mirror?"),
		  { TEXT("Its reflection"), TEXT("Its fangs"), TEXT("Its cape"), TEXT("Its shadow on the floor") },
		  TEXT("A creature without a soul, the old stories say, casts no reflection.") },
		{ TEXT("MONSTERS"), TEXT("Which shambling undead are said to crave brains?"),
		  { TEXT("Zombies"), TEXT("Ghosts"), TEXT("Vampires"), TEXT("Witches") },
		  TEXT("The word comes from Haitian folklore, where a zombie is a body raised and controlled by a sorcerer.") },
		{ TEXT("MONSTERS"), TEXT("Which monster is wrapped from head to toe in bandages?"),
		  { TEXT("A mummy"), TEXT("A zombie"), TEXT("A ghost"), TEXT("A skeleton") },
		  TEXT("Ancient Egyptians wrapped their dead in linen to preserve them for the afterlife.") },
		{ TEXT("MONSTERS"), TEXT("Mummies are most famous from which ancient land?"),
		  { TEXT("Egypt"), TEXT("Greece"), TEXT("Rome"), TEXT("China") },
		  TEXT("Some Egyptian mummies are more than 4,000 years old.") },
		{ TEXT("MONSTERS"), TEXT("What brings Frankenstein's creature to life?"),
		  { TEXT("Lightning"), TEXT("A magic spell"), TEXT("Moonlight"), TEXT("A potion") },
		  TEXT("Films made it a lightning storm; Mary Shelley's book only hints at a spark of life.") },
		{ TEXT("MONSTERS"), TEXT("Who carries a scythe and comes for the souls of the dead?"),
		  { TEXT("The Grim Reaper"), TEXT("The Sandman"), TEXT("Jack Frost"), TEXT("The Tooth Fairy") },
		  TEXT("The scythe harvests souls the way a farmer harvests wheat.") },
		{ TEXT("MONSTERS"), TEXT("Whose gaze turned people to stone?"),
		  { TEXT("Medusa"), TEXT("Cleopatra"), TEXT("Athena"), TEXT("Pandora") },
		  TEXT("Perseus beheaded her by looking only at her reflection in his shield.") },
		{ TEXT("MONSTERS"), TEXT("How many heads does Cerberus, the hound of the underworld, have?"),
		  { TEXT("Three"), TEXT("Two"), TEXT("Seven"), TEXT("One") },
		  TEXT("He lets the dead in and never lets them out.") },
		{ TEXT("MONSTERS"), TEXT("Which Egyptian god of the dead has the head of a jackal?"),
		  { TEXT("Anubis"), TEXT("Ra"), TEXT("Zeus"), TEXT("Thor") },
		  TEXT("He weighed the hearts of the dead against a feather.") },

		// --- Dark folklore ---
		{ TEXT("DARK FOLKLORE"), TEXT("Which Irish spirit's wail warns that someone will die?"),
		  { TEXT("The banshee"), TEXT("The leprechaun"), TEXT("The mermaid"), TEXT("The elf") },
		  TEXT("Bean sí means \"woman of the fairy mound\".") },
		{ TEXT("DARK FOLKLORE"), TEXT("Which witch of Russian tales lives in a hut on chicken legs?"),
		  { TEXT("Baba Yaga"), TEXT("Morgan le Fay"), TEXT("The Wicked Witch"), TEXT("Maleficent") },
		  TEXT("She flies in a mortar, steers with a pestle and sweeps away her tracks with a broom.") },
		{ TEXT("DARK FOLKLORE"), TEXT("Which horned creature goes with Saint Nicholas to punish naughty children?"),
		  { TEXT("Krampus"), TEXT("The Grinch"), TEXT("A troll"), TEXT("A goblin") },
		  TEXT("On Krampus Night, December 5, people in fur and masks still run through Alpine towns.") },
		{ TEXT("DARK FOLKLORE"), TEXT("Ghostly lights over marshes that lead travellers astray are called what?"),
		  { TEXT("Will-o'-the-wisps"), TEXT("Fireflies"), TEXT("Shooting stars"), TEXT("Northern lights") },
		  TEXT("Follow one, the stories say, and you sink into the bog.") },
		{ TEXT("DARK FOLKLORE"), TEXT("Which horseman of legend rides without a head?"),
		  { TEXT("The Headless Horseman"), TEXT("The Black Knight"), TEXT("The Pale Rider"), TEXT("The Green Knight") },
		  TEXT("Washington Irving set him in the village of Sleepy Hollow, New York, in 1820.") },
		{ TEXT("DARK FOLKLORE"), TEXT("Which ghost ship is doomed to sail the seas forever?"),
		  { TEXT("The Flying Dutchman"), TEXT("The Titanic"), TEXT("The Mayflower"), TEXT("The Black Pearl") },
		  TEXT("Sailors said seeing her on the horizon meant disaster was coming.") },
		{ TEXT("DARK FOLKLORE"), TEXT("Which plant's root was said to scream when pulled up?"),
		  { TEXT("Mandrake"), TEXT("Carrot"), TEXT("Garlic"), TEXT("Parsnip") },
		  TEXT("Harry Potter's herbology class wore earmuffs for a reason.") },
		{ TEXT("DARK FOLKLORE"), TEXT("A group of witches who meet together is called a what?"),
		  { TEXT("A coven"), TEXT("A flock"), TEXT("A choir"), TEXT("A pack") },
		  TEXT("Tradition says a coven has thirteen members.") },
		{ TEXT("DARK FOLKLORE"), TEXT("In the Prague legend, what did a rabbi make from clay to protect his people?"),
		  { TEXT("A golem"), TEXT("A dragon"), TEXT("A robot"), TEXT("A gargoyle") },
		  TEXT("One word written on its brow brought it to life; rubbing a letter out returned it to dust.") },
		{ TEXT("DARK FOLKLORE"), TEXT("Which weeping ghost of Mexican legend searches the rivers for her children?"),
		  { TEXT("La Llorona"), TEXT("La Catrina"), TEXT("La Bamba"), TEXT("La Luna") },
		  TEXT("Children are warned not to wander near rivers after dark.") },
		{ TEXT("DARK FOLKLORE"), TEXT("Which Scottish water horse drowns anyone who rides it?"),
		  { TEXT("The kelpie"), TEXT("Nessie"), TEXT("The unicorn"), TEXT("The selkie") },
		  TEXT("Its skin is sticky: once you climb on, you cannot get off before it plunges into the loch.") },

		// --- Gothic tales ---
		{ TEXT("GOTHIC TALES"), TEXT("Who wrote Dracula?"),
		  { TEXT("Bram Stoker"), TEXT("Stephen King"), TEXT("Edgar Allan Poe"), TEXT("Charles Dickens") },
		  TEXT("An Irish theatre manager, he wrote the novel in letters and diaries, published in 1897.") },
		{ TEXT("GOTHIC TALES"), TEXT("Who wrote Frankenstein?"),
		  { TEXT("Mary Shelley"), TEXT("Jane Austen"), TEXT("Agatha Christie"), TEXT("Emily Brontë") },
		  TEXT("She began it at eighteen, in a ghost-story contest with Lord Byron by Lake Geneva.") },
		{ TEXT("GOTHIC TALES"), TEXT("In the book, who is Frankenstein?"),
		  { TEXT("The doctor who made the creature"), TEXT("The creature itself"), TEXT("A village"), TEXT("The doctor's dog") },
		  TEXT("Victor Frankenstein is the scientist; his creation never gets a name.") },
		{ TEXT("GOTHIC TALES"), TEXT("Dr Jekyll turns into which evil self?"),
		  { TEXT("Mr Hyde"), TEXT("Mr Hood"), TEXT("Mr Black"), TEXT("Mr Shade") },
		  TEXT("Robert Louis Stevenson said the story came to him in a nightmare.") },
		{ TEXT("GOTHIC TALES"), TEXT("In Poe's poem, what does the raven keep saying?"),
		  { TEXT("Nevermore"), TEXT("Forever"), TEXT("Beware"), TEXT("Goodbye") },
		  TEXT("The poem made Edgar Allan Poe famous almost overnight in 1845.") },
		{ TEXT("GOTHIC TALES"), TEXT("In Poe's \"The Tell-Tale Heart\", where does the heart keep beating?"),
		  { TEXT("Under the floorboards"), TEXT("In a jar"), TEXT("In the attic"), TEXT("In a locked box") },
		  TEXT("The killer hears it louder and louder until he confesses.") },
		{ TEXT("GOTHIC TALES"), TEXT("The Headless Horseman haunts which village?"),
		  { TEXT("Sleepy Hollow"), TEXT("Salem"), TEXT("Transylvania"), TEXT("Whitby") },
		  TEXT("Next morning, all that was left of the schoolmaster was his hat and a smashed pumpkin.") },

		// --- Myth ---
		{ TEXT("MYTH"), TEXT("Who rows the dead across the river to the underworld in Greek myth?"),
		  { TEXT("Charon"), TEXT("Poseidon"), TEXT("Hercules"), TEXT("Apollo") },
		  TEXT("A coin was placed with the dead to pay him for the crossing.") },
		{ TEXT("MYTH"), TEXT("What did Orpheus do that lost his wife Eurydice forever?"),
		  { TEXT("He looked back"), TEXT("He fell asleep"), TEXT("He stopped singing"), TEXT("He spoke her name") },
		  TEXT("He had almost reached the daylight when he turned to see if she was still behind him.") },
		{ TEXT("MYTH"), TEXT("Which Greek god rules the underworld?"),
		  { TEXT("Hades"), TEXT("Zeus"), TEXT("Ares"), TEXT("Apollo") },
		  TEXT("His realm shares his name: the dead simply went \"to Hades\".") },

		// --- Day of the Dead ---
		{ TEXT("DAY OF THE DEAD"), TEXT("When is Mexico's Day of the Dead celebrated?"),
		  { TEXT("November 1 and 2"), TEXT("July 4"), TEXT("December 24"), TEXT("April 1") },
		  TEXT("Families welcome their dead home with altars, food, candles and music.") },
		{ TEXT("DAY OF THE DEAD"), TEXT("Colourful skulls made of sugar belong to which holiday?"),
		  { TEXT("The Day of the Dead"), TEXT("Easter"), TEXT("Valentine's Day"), TEXT("Thanksgiving") },
		  TEXT("Each sugar skull may carry the name of someone who has died, written in icing.") },
		{ TEXT("DAY OF THE DEAD"), TEXT("Which Pixar film is set on the Day of the Dead?"),
		  { TEXT("Coco"), TEXT("Up"), TEXT("Cars"), TEXT("Brave") },
		  TEXT("Miguel crosses a bridge of marigold petals into the Land of the Dead.") },
		{ TEXT("DAY OF THE DEAD"), TEXT("Which orange flower is said to guide the dead home?"),
		  { TEXT("The marigold"), TEXT("The rose"), TEXT("The tulip"), TEXT("The daisy") },
		  TEXT("Its bright colour and strong scent lead the spirits to their families' altars.") },

		// --- Mardi Gras and New Orleans ---
		{ TEXT("MARDI GRAS"), TEXT("What does \"Mardi Gras\" mean?"),
		  { TEXT("Fat Tuesday"), TEXT("Grand Parade"), TEXT("Masked Monday"), TEXT("Feast of Kings") },
		  TEXT("The last day of feasting before the fasting of Lent.") },
		{ TEXT("MARDI GRAS"), TEXT("Which city is most famous for its Mardi Gras?"),
		  { TEXT("New Orleans"), TEXT("New York"), TEXT("Las Vegas"), TEXT("Chicago") },
		  TEXT("Parades roll through the city for weeks before the big day.") },
		{ TEXT("MARDI GRAS"), TEXT("What do people catch from the floats at a Mardi Gras parade?"),
		  { TEXT("Beads"), TEXT("Fish"), TEXT("Snowballs"), TEXT("Letters") },
		  TEXT("Shout \"Throw me something, mister!\" and hold your hands up high.") },
		{ TEXT("MARDI GRAS"), TEXT("What are the colours of Mardi Gras?"),
		  { TEXT("Purple, green and gold"), TEXT("Orange and black"), TEXT("Red, white and blue"), TEXT("Pink and silver") },
		  TEXT("Purple for justice, green for faith and gold for power.") },
		{ TEXT("MARDI GRAS"), TEXT("What do many people wear at Mardi Gras to hide their faces?"),
		  { TEXT("Masks"), TEXT("Sunglasses"), TEXT("Scarves"), TEXT("Paper bags") },
		  TEXT("Behind a mask, anyone could be anyone for one day.") },
		{ TEXT("MARDI GRAS"), TEXT("Which cake hides a tiny baby figure for one lucky guest?"),
		  { TEXT("King cake"), TEXT("Cheesecake"), TEXT("Carrot cake"), TEXT("Cupcake") },
		  TEXT("Whoever finds the baby has to bring the next cake.") },
		{ TEXT("MARDI GRAS"), TEXT("Mardi Gras is the day before which day?"),
		  { TEXT("Ash Wednesday"), TEXT("Christmas Day"), TEXT("New Year's Day"), TEXT("Halloween") },
		  TEXT("At midnight the police clear Bourbon Street and Carnival is over until next year.") },
		{ TEXT("MARDI GRAS"), TEXT("The decorated vehicles in a Mardi Gras parade are called what?"),
		  { TEXT("Floats"), TEXT("Boats"), TEXT("Wagons"), TEXT("Sleighs") },
		  TEXT("Some are built all year long, in giant warehouses called dens.") },
		{ TEXT("MARDI GRAS"), TEXT("The clubs that put on the Mardi Gras parades are called what?"),
		  { TEXT("Krewes"), TEXT("Gangs"), TEXT("Teams"), TEXT("Bands") },
		  TEXT("Many keep their kings and queens a secret until the day.") },
		{ TEXT("NEW ORLEANS"), TEXT("Which music was born in New Orleans?"),
		  { TEXT("Jazz"), TEXT("Reggae"), TEXT("Opera"), TEXT("K-pop") },
		  TEXT("Louis Armstrong grew up there, playing the cornet in the streets.") },
		{ TEXT("NEW ORLEANS"), TEXT("What is the sugar-dusted fried pastry of New Orleans cafés?"),
		  { TEXT("The beignet"), TEXT("The croissant"), TEXT("The bagel"), TEXT("The waffle") },
		  TEXT("Served three to a plate under a snowdrift of powdered sugar.") },
		{ TEXT("NEW ORLEANS"), TEXT("Why are the dead of New Orleans buried above ground?"),
		  { TEXT("The ground is too wet"), TEXT("It is the law"), TEXT("To scare away thieves"), TEXT("The ground is solid rock") },
		  TEXT("The city sits so low that a buried coffin could float up after heavy rain.") },
		{ TEXT("NEW ORLEANS"), TEXT("Who was the famous \"Voodoo Queen\" of old New Orleans?"),
		  { TEXT("Marie Laveau"), TEXT("Marie Antoinette"), TEXT("Queen Victoria"), TEXT("Madame Tussaud") },
		  TEXT("Visitors still leave offerings at her tomb in St. Louis Cemetery No. 1.") },
	};
	return List;
}

bool IsHalloweenEventRiddle(const FPuzzleRiddle& Riddle)
{
	static const TCHAR* const Excluded[] = { TEXT("DAY OF THE DEAD"), TEXT("MARDI GRAS"), TEXT("NEW ORLEANS") };
	for (const TCHAR* Topic : Excluded)
	{
		if (FCString::Strcmp(Riddle.Topic, Topic) == 0)
		{
			return false;
		}
	}
	return true;
}

const TArray<int32>& HalloweenEventRiddleIndices()
{
	static const TArray<int32> Indices = []()
	{
		TArray<int32> Result;
		const TArray<FPuzzleRiddle>& All = PuzzleRiddles();
		for (int32 Index = 0; Index < All.Num(); ++Index)
		{
			if (IsHalloweenEventRiddle(All[Index]))
			{
				Result.Add(Index);
			}
		}
		return Result;
	}();
	return Indices;
}

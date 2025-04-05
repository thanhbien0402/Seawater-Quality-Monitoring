import { redirect } from 'next/navigation'
import { Navbar } from '../components/Navbar'
import SplineComponent from '../components/SplineComponent'
import readUserSession from '@/actions'
import { Header } from '@/components/Header'
import { Footer } from '@/components/Footer'

export default async function Home() {
  const { data } = await readUserSession()

  if (!data.session) {
    return redirect('/auth')
  }
  return (
    <div className="bg-gradient-to-t from-[#111627] to-[#344378] h-screen">
      <div className="absolute w-full h-full text-white flex flex-col justify-between">
        <div className="flex flex-col">
          <Navbar />
          <Header />
        </div>
        <Footer />
      </div>
      <SplineComponent />
    </div>
  )
}
